/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include <zlib.h>

#include "ZipCompressionFilter.h"

void ZipCompressionFilter::decompress(Packet* pack) {
	char* pData = pack->getBuffer();
	int nLength = pack->size();

	uint16 opcode = *(uint16*)pData;
	uint16 crc = *(uint16*)(pData+nLength-2);

	char output[COMPRESSION_BUFFER_MAX];
	char* outputPtr = output;
	unsigned long newLength = 0;

	uint16 offset = 1;
	if ((uint8)opcode == 0x00) //put offset for standalone or soe_opcoded pkt
		offset = 2;

	z_stream packet;
	packet.zalloc = Z_NULL;
	packet.zfree = Z_NULL;
	packet.opaque = Z_NULL;
	packet.avail_in = 0;
	packet.next_in = Z_NULL;
	inflateInit(&packet);
	packet.next_in = (Bytef* )(pData+offset);
	packet.avail_in = (nLength-offset-3);
	//System::out << "WTF - " << offset << " - " << (nLength-offset-2) << "\n";
	packet.next_out = (Bytef* )output;
	packet.avail_out = COMPRESSION_BUFFER_MAX;
	inflate(&packet,Z_FINISH);
	newLength = packet.total_out;
	inflateEnd(&packet); //close buffer*/

	//System::out << "size = " << newLength << "\n";

	pack->reset();
	pack->setSize(newLength + offset + 3, false);

	if (offset == 2) {
		pack->insertShort(opcode);
	} else {
		pack->insertByte((uint8) opcode);
	}

	pack->insertStream(outputPtr, newLength);

	pack->insertByte(0x01); //set compression flag
	pack->insertShort(crc);

	pack->setOffset(2);
}

bool ZipCompressionFilter::compress(Packet* pack) {
	char* pData = pack->getBuffer();
	int nLength = pack->size();

	uint16 opcode = *(uint16*)pData;
	uint16 crc = *(uint16*)(pData+nLength-2);

	char *output = new char[nLength+20]; //size + 20 for zlib header/footers in worst case scenerio

	uint16 offset;
	if ((uint8)opcode == 0x00)
		offset = 2;
	else
		offset = 1;

	z_stream packet;
	packet.zalloc = Z_NULL;
	packet.zfree = Z_NULL;
	packet.opaque = Z_NULL;
	packet.avail_in = 0;
	packet.next_in = Z_NULL;
	deflateInit(&packet, Z_DEFAULT_COMPRESSION);
	packet.next_in = (Bytef* )(pData+offset);
	packet.avail_in = nLength - offset - 3;
	packet.next_out = (Bytef* )output;
	packet.avail_out = nLength + 20;
	deflate(&packet,  Z_FINISH);
	int compSize = packet.total_out;

	if (compSize + 3 >= nLength) {
	//if (false) {
		delete [] output;

		return false; //We didn't compress it.
	} else {
		pack->reset();
		pack->setSize(compSize + offset + 3, false);

		if (offset == 2) {
			pack->insertShort(opcode);
		} else {
			pack->insertByte((uint8) opcode);
		}

		pack->insertStream(output, compSize);

		pack->insertByte(0x01); //set compression flag
		pack->insertShort(crc);

		pack->reset();

		deflateEnd(&packet);

		delete [] output;
		return true;
	}

	return false; //We didn't compress it.
}

