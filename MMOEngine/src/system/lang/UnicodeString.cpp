/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/platform.h"

#include "String.h"
#include "StringBuffer.h"
#include "Math.h"
#include "UnicodeString.h"

#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

UnicodeString::UnicodeString() : Variable() {
	create("", 0);
}

UnicodeString::UnicodeString(const char* ascii) : Variable() {
	auto len = strlen(ascii);
	create(ascii, len);
}

UnicodeString::UnicodeString(const String& ascii) : Variable() {
	create(ascii.toCharArray(), ascii.length());
}

UnicodeString::UnicodeString(const char* ascii, int len) : Variable() {
	create(ascii, len);
}

UnicodeString::UnicodeString(const UnicodeString& str) : Variable() {
	uString = new UnicodeCharType[str.count + 1];
	count = str.count;

	copy(uString, str.uString);
}

UnicodeString::UnicodeString(UnicodeString&& str) : Variable(), uString(str.uString), count(str.count) {
	str.uString = nullptr;
	str.count = 0;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& str) {
	if (this == &str) {
		return *this;
	}

	if (uString != nullptr) {
		delete [] uString;
	}

	uString = str.uString;
	count = str.count;

	str.uString = nullptr;
	str.count = 0;

	return *this;
}

UnicodeString::~UnicodeString() {
	if (uString != nullptr) {
		delete [] uString;
	}
}

void UnicodeString::create(const char* ascii, int len) {
	//mbstate_t state;
	//memset(&state, '\0', sizeof (state));

	uString = new UnicodeCharType[len + 1];
	//mbsrtowcs(uString, &ascii, len, &state);
	asciitowide(uString, ascii, len);

	uString[count = len] = 0;
}

void UnicodeString::asciitowide(UnicodeCharType* UnicodeString, const char* ascii, int len) {
	for (int i = 0; i < len; ++i)
		UnicodeString[i] = ascii[i];
}

void UnicodeString::copy(UnicodeCharType* dest, const UnicodeCharType* src) {
	for (int i = 0; true; ++i) {
		UnicodeCharType chr;
		memcpy(&chr, src + i, sizeof(chr));

		dest[i] = chr;

		if (chr == 0)
			break;
	}
}

void UnicodeString::copy(UnicodeCharType* dest, const UnicodeCharType* src, int len) {
	for (int i = 0; i < len; ++i) {
		UnicodeCharType chr;
		memcpy(&chr, src + i, sizeof(chr));

		dest[i] = chr;

		if (chr == 0)
			break;
	}
}

UnicodeString& UnicodeString::operator=(const UnicodeString& str) {
	if (this != &str) {
		delete [] uString;

		count = str.count;
		uString = new UnicodeCharType[count + 1];

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

UnicodeString UnicodeString::concat(const UnicodeString& str) const {
	UnicodeString newStr(*this);

	newStr.append(str);

	return newStr;
}

int UnicodeString::compareTo(const UnicodeString& str) const {
	auto n = Math::min(str.length(), count);

	auto* s1 = str.uString;
	auto* s2 = uString;

	while (n-- != 0) {
		if (*s1 < *s2) return -1;
		if (*s1 > *s2) return 1;
		++s1; ++s2;
	}

	if (str.length() != count) {
		if (str.length() > count)
			return 1;
		else
			return -1;
	}

	return 0;
}

unsigned char UnicodeString::operator[](int index) const {
	return uString[index];
}

void UnicodeString::append(const String& ascii) {
	append(ascii.toCharArray(), ascii.length());
}

void UnicodeString::append(const UnicodeString& uni) {
	append(uni.toWideCharArray(), uni.length());
}

void UnicodeString::append(const char* ascii) {
	auto len = strlen(ascii);

	append(ascii, len);
}

void UnicodeString::append(const char* ascii, int len) {
	//mbstate_t state;
  	//memset (&state, '\0', sizeof (state));

  	auto ncount = count + len;
	auto nuString = new UnicodeCharType[ncount + 1];

	//wmemcpy(nuString, uString, count);
	copy(nuString, uString, count);
	delete [] uString;

	//mbsrtowcs(nuString + count, &ascii, len, &state);
	asciitowide(nuString + count, ascii, len);

	uString = nuString;
	uString[count = ncount] = 0;
}

void UnicodeString::append(const UnicodeCharType* str, int len) {
	//mbstate_t state;
    //memset (&state, '\0', sizeof (state));

  	auto ncount = count + len;
	auto* nuString = new UnicodeCharType[ncount + 1];

	//wmemcpy(nuString, uString, count);
	copy(nuString, uString, count);
	delete [] uString;

	//wmemcpy(nuString + count, str, len);
	copy(nuString + count, str, len);

	uString = nuString;
	uString[count = ncount] = 0;
}

int UnicodeString::indexOf(UnicodeCharType chr) const {
	for (uint32 i = 0; i < count; ++i) {
		if (uString[i] == chr)
			return i;
	}

	return -1;
}

int UnicodeString::indexOf(const UnicodeString& str, int startPos) const {
	if (str.isEmpty() || startPos >= count || count < str.length())
		return -1;

	constexpr static const int maxLoops = 2048;

	for (uint32 i = startPos; i <= count - str.length() && i < maxLoops; ++i) {
		if (!memcmp(uString + i, str.toWideCharArray(), str.length() * sizeof(UnicodeCharType)))
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
	delete [] uString;

	create("", 0);
}

const char* UnicodeString::toCharArray() const {
	return reinterpret_cast<const char*>(uString);
}

const UnicodeString::UnicodeCharType* UnicodeString::toWideCharArray() const {
	return uString;
}

bool UnicodeString::toString(String& ascii) const {
	StringBuffer str;

	str << '"';

	str << toString();

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

	stream->writeStream(toCharArray(), count * UnicodeCharSize::value);

	return true;
}

bool UnicodeString::parseFromBinaryStream(ObjectInputStream* stream) {
	uint32 len = stream->readInt();
	auto buffer = stream->getBuffer() + stream->getOffset();

	stream->shiftOffset(len * UnicodeCharSize::value);

	if (uString) {
		delete [] uString;
	}

	uString = new UnicodeCharType[len + 1];
	copy(uString, reinterpret_cast<const UnicodeCharType*>(buffer), len);

	uString[count = len] = 0;

	return true;
}

UnicodeString UnicodeString::replaceFirst(const UnicodeString& regex, const UnicodeString& replacement) const {
	auto rlen = regex.count;

	int i = indexOf(regex);

	if (i != -1) {
		if (i > 0 && i + rlen < count)
			return subString(0, i) + replacement + subString(i + rlen, count);
		else if (i == 0 && i + rlen < count)
			return replacement + subString(i + rlen, count);
		else if (i == 0 && i + rlen >= count)
			return replacement;
		else if (i > 0 && i + rlen == count)
			return subString(0, i) + replacement;
		else
			return regex;
	} else
		return *this;
}