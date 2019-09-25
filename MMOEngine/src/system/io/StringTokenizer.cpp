/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/lang/Integer.h"
#include "system/lang/Long.h"

#include "StringTokenizer.h"

StringTokenizer::StringTokenizer(const String& s) {
	str = s;

	if (!s.isEmpty())
		index = 0;
	else
		index = -1;

	delimiter = " ";
}

int StringTokenizer::getIntToken() {
	String token;
	nextToken(token);

	return Integer::valueOf(token);
}

uint32 StringTokenizer::getHexIntToken() {
	String token;
	nextToken(token);

	return Integer::hexvalueOf(token);
}

float StringTokenizer::getFloatToken() {
	String token;
	nextToken(token);

	return (float) atof(token.toCharArray());
}

int64 StringTokenizer::getLongToken() {
	String token;
	nextToken(token);

	return Long::valueOf(token);
}

uint64 StringTokenizer::getUnsignedLongToken() {
	String token;
	nextToken(token);

	return UnsignedLong::valueOf(token);
}

void StringTokenizer::getStringToken(String& token) {
	nextToken(token);
}

void StringTokenizer::getStringToken(StringBuffer& token) {
	nextToken(token);
}

void StringTokenizer::nextToken(String& s) {
	if (!hasMoreTokens())
		throw Exception("no more tokens in " + str);

	StringBuffer buf;

	int oindex;

	while (true) {
		oindex = index;

		index = str.indexOf(delimiter, index);

		if (index == oindex && index <= str.length())
			++index;
		else
			break;
	}

	if (index != -1) {
		buf.append(str.subString(oindex, index));
		index += delimiter.length();
	} else
		buf.append(str.subString(oindex, str.length()));

	buf.toString(s);
}

void StringTokenizer::nextToken(StringBuffer& s) {
	if (!hasMoreTokens())
		throw Exception("no more tokens in " + str);

	int oindex;

	while (true) {
		oindex = index;

		index = str.indexOf(delimiter, index);

		if (index == oindex && index <= str.length())
			++index;
		else
			break;
	}

	s.deleteAll();

	if (index != -1) {
		s.append(str.subString(oindex, index));
		index += delimiter.length();
	} else
		s.append(str.subString(oindex, str.length()));
}

void StringTokenizer::finalToken(String& s) {
	if (!hasMoreTokens())
		throw Exception();

	s = str.subString(index, str.length());

	index = -1;
}

void StringTokenizer::shiftTokens(int count) {
	String token;

	while (count-- > 0)
		nextToken(token);
}

bool StringTokenizer::hasMoreTokens() const {
	return index != -1;
}
