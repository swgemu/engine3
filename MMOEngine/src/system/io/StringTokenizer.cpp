/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../lang/String.h"

#include "StringTokenizer.h"

StringTokenizer::StringTokenizer(const string& s) {
	str = s;

	if (s.length() != 0)
		index = 0;
	else
		index = string::npos;

	delimeter = " ";
}

int StringTokenizer::getIntToken() {
	string token;
	nextToken(token);
		
	return atoi(token.c_str());
}

uint32 StringTokenizer::getHexIntToken() {
	string token;
	nextToken(token);

	return String::toHexInt(token.c_str());
}

float StringTokenizer::getFloatToken() {
	string token;
	nextToken(token);
		
	return (float) atof(token.c_str());
}
	
uint64 StringTokenizer::getLongToken() {
	string token;
	nextToken(token);
		
	return String::toUnsignedLong(token.c_str());
}

void StringTokenizer::getStringToken(string& token) {
	nextToken(token);
}

void StringTokenizer::getStringToken(stringstream& token) {
	nextToken(token);
}

void StringTokenizer::nextToken(string& s) {
	if (!hasMoreTokens())
		throw Exception();

	string::size_type oindex;

	while (true) {
		oindex = index;
		
		index = str.find(delimeter.c_str(), index);

		if (index == oindex && index <= str.size())
			++index;
		else
			break;
	}

	s.clear();
	if (index != string::npos)
		s.append(str.substr(oindex, index++ - oindex));
	else
		s.append(str.substr(oindex, str.size() - oindex));
}

void StringTokenizer::nextToken(stringstream& s) {
	if (!hasMoreTokens())
		throw Exception();

	string::size_type oindex;

	while (true) {
		oindex = index;
		
		index = str.find(delimeter.c_str(), index);

		if (index == oindex && index <= str.size())
			++index;
		else
			break;
	}
	
	s.clear();
	if (index != string::npos)
		s << str.substr(oindex, index++ - oindex);
	else
		s << str.substr(oindex, str.size() - oindex);
}

void StringTokenizer::finalToken(string& s) {
	if (!hasMoreTokens())
		throw Exception();

	s.clear();
	s.append(str.substr(index, str.size() - index));
	
	index = string::npos;
}

void StringTokenizer::shiftTokens(int count) {
	string token;

	while (count-- > 0)
		nextToken(token);
}

bool StringTokenizer::hasMoreTokens() {
	return index != string::npos;
}
