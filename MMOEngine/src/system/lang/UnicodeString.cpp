/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../platform.h"

#include "String.h"
#include "StringBuffer.h"

#include "UnicodeString.h"

#include "ArrayIndexOutOfBoundsException.h"

#include "../io/ObjectOutputStream.h"
#include "../io/ObjectInputStream.h"

UnicodeString::UnicodeString() : Variable() {
	create("", 0);
}

UnicodeString::UnicodeString(const char* ascii) : Variable() {
	int len = strlen(ascii);
	create(ascii, len);
}

UnicodeString::UnicodeString(const String& ascii) : Variable() {
	create(ascii.toCharArray(), ascii.length());
}

UnicodeString::UnicodeString(const char* ascii, int len) : Variable() {
	create(ascii, len);
}

UnicodeString::UnicodeString(const UnicodeString& str) : Variable() {
	uString = new unsigned short[str.count + 1];
	count = str.count;
	//wcscpy(uString, str.uString);
	copy(uString, str.uString);
}

UnicodeString::~UnicodeString() {
	delete[] uString;
}

void UnicodeString::create(const char* ascii, int len) {
	//mbstate_t state;
	//memset(&state, '\0', sizeof (state));

	uString = new unsigned short[len + 1];
	//mbsrtowcs(uString, &ascii, len, &state);
	asciitowide(uString, ascii, len);

	uString[count = len] = 0;
}

void UnicodeString::asciitowide(unsigned short* UnicodeString, const char* ascii, int len) {
	for (int i = 0; i < len; ++i)
		UnicodeString[i] = ascii[i];
}

void UnicodeString::copy(unsigned short* dest, const unsigned short* src) {
	for (int i = 0; true; ++i) {
		unsigned short chr = src[i];

		dest[i] = chr;

		if (chr == 0)
			break;
	}
}

void UnicodeString::copy(unsigned short* dest, const unsigned short* src, int len) {
	for (int i = 0; i < len; ++i) {
		unsigned short chr = src[i];

		dest[i] = chr;

		if (chr == 0)
			break;
	}
}

UnicodeString& UnicodeString::operator=(const UnicodeString& str) {
	if (this != &str) {
		delete[] uString;

		count = str.count;
		uString = new unsigned short[count + 1];

		//wcscpy(uString, str.uString);
		copy(uString, str.uString);
	}

	return *this;
}

UnicodeString& UnicodeString::operator=(const String& ascii) {
	clear();
	append(ascii);

	return *this;
}

UnicodeString& UnicodeString::operator=(const char* ascii) {
	clear();
	append(ascii);

	return *this;
}

UnicodeString& UnicodeString::operator+(const String& ascii) {
	append(ascii);
	return *this;
}

UnicodeString& UnicodeString::operator+(const char* ascii) {
	append(ascii);

	return *this;
}

UnicodeString& UnicodeString::operator+(const UnicodeString& str) {
	append(str);
	return *this;
}

char UnicodeString::operator[](int index) const {
	return (char) uString[index];
}

void UnicodeString::append(const String& ascii) {
	append(ascii.toCharArray(), ascii.length());
}

void UnicodeString::append(const UnicodeString& uni) {
	append((unsigned short*) uni.toWideCharArray(), uni.length());
}

void UnicodeString::append(const char* ascii) {
	int len = strlen(ascii);
	append(ascii, len);
}

void UnicodeString::append(const char* ascii, int len) {
	//mbstate_t state;
  	//memset (&state, '\0', sizeof (state));

  	int ncount = count + len;
	unsigned short *nuString = new unsigned short[ncount + 1];

	//wmemcpy(nuString, uString, count);
	copy(nuString, uString, count);
	delete[] uString;

	//mbsrtowcs(nuString + count, &ascii, len, &state);
	asciitowide(nuString + count, ascii, len);

	uString = nuString;
	uString[count = ncount] = 0;
}

void UnicodeString::append(const unsigned short* str, int len) {
	//mbstate_t state;
    //memset (&state, '\0', sizeof (state));

  	int ncount = count + len;
	unsigned short *nuString = new unsigned short[ncount + 1];

	//wmemcpy(nuString, uString, count);
	copy(nuString, uString, count);
	delete[] uString;

	//wmemcpy(nuString + count, str, len);
	copy(nuString + count, str, len);

	uString = nuString;
	uString[count = ncount] = 0;
}

int UnicodeString::indexOf(unsigned short chr) const {
	for (int i = 0; i < count; ++i) {
		if (uString[i] == chr)
			return i;
	}

	return -1;

	/*unsigned short* pos = wcschr(uString, chr);

	if (pos != NULL)
		return (int) (pos - uString);
	else
		return -1;*/
}

int UnicodeString::indexOf(const UnicodeString& str, int startPos) const {
	if (str.isEmpty())
		return -1;

	for (int i = startPos; i <= count - str.length(); ++i) {
		if (!memcmp(uString + i, str.toWideCharArray(), str.length() * sizeof(unsigned short)))
			return i;
	}

	return -1;
}

UnicodeString UnicodeString::subString(int beg, int end) const {
	UnicodeString nstr;
	nstr.append(uString + beg, end - beg);

	return nstr;
}

UnicodeString UnicodeString::substr(int beg, int length) const {
	UnicodeString nstr;

	if (length == 0)
		return nstr;

	nstr.append(uString + beg, length);

	return nstr;
}

void UnicodeString::clear() {
	delete[] uString;

	create("", 0);
}

const unsigned short* UnicodeString::toWideCharArray() const {
	return uString;
}

String UnicodeString::toString() const {
	StringBuffer ascii;

	for (int i = 0; i < count; ++i)
		ascii.append((char*) (uString + i), 1);

	return ascii.toString();
}

bool UnicodeString::toString(String& ascii) {
	StringBuffer str;

	str << '"';

	for (int i = 0; i < count; ++i)
		str.append((char*) (uString + i), 1);

	str << '"';

	ascii = str.toString();

	return true;
}

bool UnicodeString::parseFromString(const String& str, int version) {
	String data;
	data.parseFromString(str);

	*this = data;

	return true;
}

bool UnicodeString::toBinaryStream(ObjectOutputStream* stream) {
	stream->writeInt(count);

	stream->writeStream((char*)uString, count  * 2);

	return true;
}

bool UnicodeString::parseFromBinaryStream(ObjectInputStream* stream) {
	uint32 len = stream->readInt();

	unsigned short uni[len];

	stream->readStream((char*)uni, len * 2);

	clear();
	append(uni, len);

	return true;
}
