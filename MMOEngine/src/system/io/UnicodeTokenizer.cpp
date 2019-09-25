/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/lang/Integer.h"
#include "system/lang/Long.h"

#include "UnicodeTokenizer.h"

UnicodeTokenizer::UnicodeTokenizer(const String& s) {
	str = s;

	if (!s.isEmpty())
		index = 0;
	else
		index = -1;

	delimeter = UnicodeString(" ");
}

UnicodeTokenizer::UnicodeTokenizer(const UnicodeString& u) {
	str = u;

	if (!u.isEmpty())
		index = 0;
	else
		index = -1;

	delimeter = UnicodeString(" ");
}

UnicodeTokenizer::UnicodeTokenizer(const char* ascii) {
	str = ascii;

	if (!str.isEmpty())
		index = 0;
	else
		index = -1;

	delimeter = UnicodeString(" ");
}

UnicodeTokenizer::UnicodeTokenizer(const char* ascii, int len) {
	str = UnicodeString(ascii, len);

	if (len != 0)
		index = 0;
	else
		index = -1;

	delimeter = UnicodeString(" ");
}

int UnicodeTokenizer::getIntToken() {
	UnicodeString token;
	nextToken(token);

	return Integer::valueOf(token.toString());
}

uint32 UnicodeTokenizer::getHexIntToken() {
	UnicodeString token;
	nextToken(token);

	return Integer::hexvalueOf(token.toString());
}

float UnicodeTokenizer::getFloatToken() {
	UnicodeString token;
	nextToken(token);

	return (float) atof(token.toString().toCharArray());
}

int64 UnicodeTokenizer::getLongToken() {
	UnicodeString token;
	nextToken(token);

	return Long::valueOf(token.toString());
}

uint64 UnicodeTokenizer::getUnsignedLongToken() {
	UnicodeString token;
	nextToken(token);

	return Long::unsignedvalueOf(token.toString());
}

void UnicodeTokenizer::getStringToken(String& token) {
	UnicodeString tok;
	nextToken(tok);

	token = tok.toString();
}

void UnicodeTokenizer::getUnicodeToken(UnicodeString& token) {
	nextToken(token);
}

void UnicodeTokenizer::getStringToken(StringBuffer& token) {
	UnicodeString tok;
	nextToken(tok);

	token.deleteAll();
	token.append(tok.toString());
}

void UnicodeTokenizer::nextToken(UnicodeString& s) {
	if (!hasMoreTokens())
		throw Exception();

	int oindex;

	while (true) {
		oindex = index;

		index = str.indexOf(delimeter, index);

		if (index == oindex && index <= str.length())
			++index;
		else
			break;
	}

	s.clear();

	if (index != -1) {
		s.append(str.substr(oindex, index - oindex));
		index += delimeter.length();
	} else {
		s.append(str.substr(oindex, str.length() - oindex));
	}
}

void UnicodeTokenizer::finalToken(String& s) {
	if (!hasMoreTokens())
		throw Exception();

	s = str.substr(index, str.length() - index).toString();

	index = -1;
}

void UnicodeTokenizer::finalToken(UnicodeString& s) {
	if (!hasMoreTokens())
		throw Exception();

	s = str.substr(index, str.length() - index);

	index = -1;
}

void UnicodeTokenizer::shiftTokens(int count) {
	UnicodeString token;

	while (count-- > 0)
		nextToken(token);
}

bool UnicodeTokenizer::hasMoreTokens() const {
	return index != -1;
}
