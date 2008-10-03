/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../lang/String.h"

#include "UnicodeTokenizer.h"

UnicodeTokenizer::UnicodeTokenizer(const string& s) {
	str = s;

	if (s.length() != 0)
		index = 0;
	else
		index = -1;

	delimeter = unicode(" ");
}

UnicodeTokenizer::UnicodeTokenizer(const unicode& u) {
	str = u;

	if (u.size() != 0)
		index = 0;
	else
		index = -1;

	delimeter = unicode(" ");
}

UnicodeTokenizer::UnicodeTokenizer(const char* ascii) {
	str = ascii;

	if (str.size() != 0)
		index = 0;
	else
		index = -1;

	delimeter = unicode(" ");
}

UnicodeTokenizer::UnicodeTokenizer(const char* ascii, int len) {
	str = unicode(ascii, len);

	if (len != 0)
		index = 0;
	else
		index = -1;

	delimeter = unicode(" ");
}

int UnicodeTokenizer::getIntToken() {
	unicode token;
	nextToken(token);

	return atoi(token.c_str().c_str());
}

uint32 UnicodeTokenizer::getHexIntToken() {
	unicode token;
	nextToken(token);

	return String::toHexInt(token.c_str().c_str());
}

float UnicodeTokenizer::getFloatToken() {
	unicode token;
	nextToken(token);

	return (float) atof(token.c_str().c_str());
}

uint64 UnicodeTokenizer::getLongToken() {
	unicode token;
	nextToken(token);

	return String::toUnsignedLong(token.c_str().c_str());
}

void UnicodeTokenizer::getStringToken(string& token) {
	unicode tok;
	nextToken(tok);

	token.clear();
	token.append(tok.c_str());
}

void UnicodeTokenizer::getUnicodeToken(unicode& token) {
	nextToken(token);
}

void UnicodeTokenizer::getStringToken(stringstream& token) {
	unicode tok;
	nextToken(tok);

	token.clear();
	token << tok.c_str();
}

void UnicodeTokenizer::nextToken(unicode& s) {
	if (!hasMoreTokens())
		throw Exception();

	int oindex;

	while (true) {
		oindex = index;

		index = str.find(delimeter, index);

		if (index == oindex && index <= str.size())
			++index;
		else
			break;
	}

	s.clear();
	if (index != -1) {
		s.append(str.substr(oindex, index++ - oindex));
	} else {
		s.append(str.substr(oindex, str.size() - oindex));
	}
}

void UnicodeTokenizer::finalToken(string& s) {
	if (!hasMoreTokens())
		throw Exception();

	s.clear();
	s.append((str.substr(index, str.size() - index)).c_str());

	index = -1;
}

void UnicodeTokenizer::finalToken(unicode& s) {
	if (!hasMoreTokens())
		throw Exception();

	s.clear();
	s.append(str.substr(index, str.size() - index));

	index = -1;
}

void UnicodeTokenizer::shiftTokens(int count) {
	unicode token;

	while (count-- > 0)
		nextToken(token);
}

bool UnicodeTokenizer::hasMoreTokens() const {
	return index != -1;
}
