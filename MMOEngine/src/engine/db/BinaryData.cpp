/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "BinaryData.h"

using namespace engine::db;

BinaryData::BinaryData(String& data) {
	iData = data.toCharArray();
	iDataSize = data.length();
}

void BinaryData::encode(String& stream) {
   	int oDataSize = 1 + (iDataSize * 8 + 5) / 6;
	char* oData = new char[oDataSize];

	sys::uint8 *cur = (sys::uint8 *)iData;
	sys::uint8 *curend = cur + iDataSize;
	sys::uint8 *out = (sys::uint8 *)oData;

	int accbits = 0;
	sys::uint8 acc = 0;

	while (cur < curend) {
    	int bits = 8;
    	sys::uint8 byte = *cur++;

    	while (accbits + bits >= 6)	{
       		// Fill missing lower bits of the accumulator
       		acc |= (byte >> (2 + accbits));
       		// Move unused bits to upper part of the byte
       		byte = (byte << (6 - accbits));
       		bits -= (6 - accbits);
       		// We have exactly 6 bits in accumulator, spit it out
       		*out++ = base64_chars [acc];
       		acc = 0;
       		accbits = 0;
    	}

    	// Keep what remained of the byte for the next round
    	accbits = bits;
    	acc = (byte >> 2);
	}

	// Spit out what we have left in accumulator
	if (accbits)
    	*out++ = base64_chars [acc];

	*out++ = 0;

	StringBuffer buf;
	buf.append(oData/*, out - (sys::uint8 *)oData*/);
	delete[] oData;

	stream = buf.toString();
}

bool BinaryData::decode(String& stream) {
	int oDataSize = (iDataSize * 3) / 4;
	char* oData = new char[oDataSize];

	const sys::uint8 *cur = (const sys::uint8 *)iData;
	sys::uint8 *out = (sys::uint8 *)oData;
	sys::uint8 *outend = out + oDataSize;

	sys::uint8 acc = 0;
	int accbits = 0;

	while (out < outend) {
    	char c = *cur++;
    	if (!c)
        	break;

        char *s = (char*) strchr (base64_chars, c);
	if (!s) {
		delete [] oData;

		return false;
	}

        sys::uint8 byte = (s - base64_chars) << 2;

    	if (accbits + 6 >= 8) {
        	acc |= byte >> accbits;
        	*out++ = acc;
        	acc = byte << (8 - accbits);
        	accbits = accbits - 2;
    	} else {
        	acc = byte;
        	accbits = 6;
    	}
	}

	StringBuffer buf;

	// Ignore any bits we could have left in the accumulator
	buf.append(oData, out - (sys::uint8 *)oData);
	delete[] oData;

	stream = buf.toString();

	return true;
}
