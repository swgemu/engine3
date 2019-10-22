/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "StringBuffer.h"

#include "Character.h"

#include <regex.h>

static_assert(String::npos == ArrayList<char>::npos, "String and StringBuffer npos values differ");

StringBuffer::StringBuffer() : ArrayList<char>(), streamFlags(SF_none) {
}

StringBuffer::StringBuffer(const String& str) : ArrayList<char>(str.length(), 0), streamFlags(SF_none) {
	append(str);
}

StringBuffer::StringBuffer(const StringBuffer& s) : ArrayList<char>(s), streamFlags(s.streamFlags) {
}

StringBuffer::StringBuffer(StringBuffer&& str) : ArrayList<char>(std::move(str)), streamFlags(str.streamFlags) {
}

StringBuffer::~StringBuffer() {
}

StringBuffer& StringBuffer::append(char ch) {
	if (doUpperCase())
		ch = Character::toUpperCase(ch);

	emplace(ch);

	return *this;
}

StringBuffer& StringBuffer::append(int val) {
	String str;

	if (!doHex())
		str = String::valueOf(val);
	else
		str = String::hexvalueOf(val);

	return append(str);
}

StringBuffer& StringBuffer::append(uint32 val) {
	String str;

	if (!doHex())
		str = String::valueOf(val);
	else
		str = String::hexvalueOf((int)val);

	return append(str);
}

StringBuffer& StringBuffer::append(long val) {
	return append((int64) val);
}

StringBuffer& StringBuffer::append(int64 val) {
	String str;

	if (!doHex())
		str = String::valueOf(val);
	else
		str = String::hexvalueOf(val);

	return append(str);
}

StringBuffer& StringBuffer::append(uint64 val) {
	String str;

	if (!doHex())
		str = String::valueOf(val);
	else
		str = String::hexvalueOf((int64)val);

	return append(str);
}

StringBuffer& StringBuffer::append(std::size_t val) {
	String str;

	if (!doHex())
		str = String::valueOf(val);
	else
		str = String::hexvalueOf((int64) val);

	return append(str);
}

StringBuffer& StringBuffer::append(float val) {
	String str = String::valueOf(val);

	return append(str);
}

StringBuffer& StringBuffer::append(double val) {
	String str = String::valueOf(val);

	return append(str);
}

StringBuffer& StringBuffer::append(bool val) {
	String str = String::valueOf(val);

	return append(str);
}

StringBuffer& StringBuffer::append(const void* val) {
	String str = String::valueOf(val);

	return append(str);
}

StringBuffer& StringBuffer::append(const char* str) {
	for (int i = 0; str[i] != 0; ++i)
		append(str[i]);

	return *this;
}

StringBuffer& StringBuffer::append(const char* str, int len) {
	for (int i = 0; i < len; ++i)
		append(str[i]);

	return *this;
}

StringBuffer& StringBuffer::append(const String& str) {
	return append(str.toCharArray(), str.length());
}

StringBuffer& StringBuffer::append(const StringBuffer& str) {
	return append(str.getBuffer(), str.length());
}

StringBuffer& StringBuffer::append(const UnicodeString& str) {
	auto val = str.toString();

	return append(val.toCharArray(), val.length());
}

void StringBuffer::deleteRange(int start, int end) {
	ArrayList<char>::removeRange(start, end);
}

void StringBuffer::deleteAll() {
	ArrayList<char>::removeAll();
}

int StringBuffer::indexOf(char ch) const  {
	return indexOf(ch, 0);
}

int StringBuffer::indexOf(char ch, int fromIndex) const {
	for (int i = fromIndex; i <= elementCount - 1; ++i) {
		if (!memcmp(elementData + i, &ch, sizeof(char)))
			return i;
	}

	return npos;
}

int StringBuffer::indexOf(const String& str) const {
	return indexOf(str, 0);
}

int StringBuffer::indexOf(const String& regexString, int& resultStart, int& resultEnd, int fromIndex) const {
	String val = toString();

	if (fromIndex >= val.length())
		throw ArrayIndexOutOfBoundsException(fromIndex);

	regex_t regex;
	regmatch_t pmatch[1];

	int reti = regcomp(&regex, regexString.toCharArray(), REG_EXTENDED);

	if (!reti) {
		reti = regexec(&regex, val.toCharArray() + fromIndex, 1, pmatch, 0);

		if (reti) {
			regfree(&regex);

			return String::npos;
		}
	} else
		return String::npos;

	regfree(&regex);

	resultStart = pmatch[0].rm_so;
	resultEnd = pmatch[0].rm_eo;

	return reti;
}

int StringBuffer::indexOf(const String& str, int fromIndex) const {
	if (str.isEmpty())
		return String::npos;

	for (int i = fromIndex; i <= elementCount - str.length(); ++i) {
		if (!memcmp(elementData + i, str.toCharArray(), str.length() * sizeof(char)))
			return i;
	}

	return String::npos;
}

StringBuffer& StringBuffer::insert(int offset, char ch) {
	if (doUpperCase())
		ch = Character::toUpperCase(ch);

	add(offset, ch);

	return *this;
}

StringBuffer& StringBuffer::insert(int offset, int val) {
	String str;

	if (!doHex())
		str = String::valueOf(val);
	else
		str = String::hexvalueOf(val);

	return insert(offset, str);
}

StringBuffer& StringBuffer::insert(int offset, uint32 val) {
	return insert(offset, (int) val);
}

StringBuffer& StringBuffer::insert(int offset, long val) {
	return insert(offset, (uint32) val);
}

StringBuffer& StringBuffer::insert(int offset, int64 val) {
	String str = String::valueOf(val);

	return insert(offset, str);
}

StringBuffer& StringBuffer::insert(int offset, uint64 val) {
	String str = String::valueOf(val);

	return insert(offset, str);
}

StringBuffer& StringBuffer::insert(int offset, float val) {
	String str = String::valueOf(val);

	return insert(offset, str);
}

StringBuffer& StringBuffer::insert(int offset, double val) {
	String str = String::valueOf(val);

	return insert(offset, str);
}

StringBuffer& StringBuffer::insert(int offset, void* val) {
	String str = String::valueOf(val);

	return insert(offset, str);
}

StringBuffer& StringBuffer::insert(int offset, const char* str) {
	int len = strlen(str);

	return insert(offset, str, len);
}

StringBuffer& StringBuffer::insert(int offset, const char* str, int len) {
	for (int i = 0; i < len; ++i)
		insert(offset + i, str[i]);

	return *this;
}

StringBuffer& StringBuffer::insert(int offset, const String& str) {
	return insert(offset, str.toCharArray(), str.length());
}

StringBuffer& StringBuffer::replace(int start, int end, const char* str) {
	deleteRange(start, end);

	insert(start, str);

	return *this;
}

StringBuffer& StringBuffer::replace(int start, int end, const String& str) {
	deleteRange(start, end);

	insert(start, str);

	return *this;
}

String StringBuffer::toString() const {
	if (!elementData) {
		return String();
	} else {
		return String(elementData, elementCount);
	}
}

void StringBuffer::toString(String& str) const {
	str = toString();
}

StringBuffer& StringBuffer::operator<< (char ch) {
	return append(ch);
}

StringBuffer& StringBuffer::operator<< (int val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (uint32 val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (long val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (int64 val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (uint64 val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (float val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (double val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (bool val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (const void* val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (std::size_t val) {
	return append(val);
}

StringBuffer& StringBuffer::operator<< (const char* str) {
	return append(str);
}

StringBuffer& StringBuffer::operator<< (const String& str) {
	return append(str.toCharArray(), str.length());
}

StringBuffer& StringBuffer::operator<< (const UnicodeString& str) {
	return append(str);
}

StringBuffer& StringBuffer::operator<< (const StringBuffer& str) {
	return append(str);
}

StringBuffer& StringBuffer::operator<< (const StreamFlags flags) {
	switch (flags) {
	case sys::lang::dec:
		streamFlags = (StreamFlags) ((uint32) streamFlags & ~(uint32) hex);
		break;
	case sys::lang::nouppercase:
		streamFlags = (StreamFlags) ((uint32) streamFlags & ~(uint32) uppercase);
		break;
	case sys::lang::endl:
		append('\n');
		break;
	default:
		streamFlags = (StreamFlags) ((uint32) streamFlags | (uint32) flags);
		break;
	}

	return *this;
}

void StringBuffer::reserve(std::size_t newCapacity) {
	ensureCapacity(newCapacity);
}

bool StringBuffer::operator==(const StringBuffer& buff) const {
	if (buff.length() != length())
		return false;

	if (buff.length() == 0) //make sure we dont pass nullptr to memcpy in case StringBuffer is empty
		return true;

	return memcmp(getBuffer(), buff.getBuffer(), length() * sizeof(char)) == 0;
}

bool StringBuffer::operator==(const String& buff) const {
	if (buff.length() != length())
		return false;

	if (buff.length() == 0)
		return true;

	return memcmp(getBuffer(), buff.toCharArray(), length() * sizeof(char)) == 0;
}

bool operator==(const String& str1, const StringBuffer& str2) {
	if (str1.length() != str2.length())
		return false;

	if (str1.length() == 0)
		return true;

	return memcmp(str1.toCharArray(), str2.getBuffer(), str1.length() * sizeof(char)) == 0;
}

bool operator!=(const String& str1, const StringBuffer& str2) {
	return !(str1 == str2);
}

