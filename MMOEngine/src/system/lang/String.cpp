/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "String.h"
#include "StringBuffer.h"

#include "Long.h"

#include <regex>

constexpr uint32 StringHashCodeTable::crctable[];

String::String() {
	create("", 0);
}

String::String(const std::string& str) {
	create(str.c_str(), str.size());
}

String::String(const ConstString& str) {
	create(str.begin(), str.size());
}

String::String(const char* str) {
	if (str == nullptr)
		throw IllegalArgumentException();

	create(str, strlen(str));
}

String::String(const char* str, int len) {
	if (len && str == nullptr)
		throw IllegalArgumentException();

	create(str, len);
}

#ifdef STRING_INHERIT_VARIABLE
String::String(const String& str) : Variable() {
#else
String::String(const String& str) {
#endif
	create(str.begin(), str.count);
}

String::~String() {
#ifdef SSO_STRING
	if (count >= SSO_SIZE) {
		free(value);
		//value = nullptr;
	}

	//count = 0;
#else
	if (value != nullptr) {
		free(value);
		value = nullptr;

		count = 0;
	}
#endif
}

#ifdef STRING_INHERIT_VARIABLE
String::String(String&& str) : Variable() {
#else
String::String(String&& str) {
#endif
	count = str.count;

#ifdef SSO_STRING
	if (count < SSO_SIZE) {
		memcpy(sso, str.sso, count);
		sso[count] = 0;
	} else {
		value = str.value;
		str.value = nullptr;
		str.count = 0;
	}
#else
	value = str.value;
	str.value = nullptr;
	str.count = 0;
#endif
}

void String::create(const char* str, int len) {
#ifdef SSO_STRING
	if (len < SSO_SIZE) {
		memcpy(sso, str, len);
		sso[len] = 0;
		count = len;
	} else {
#endif
	value = (char*) malloc(len + 1);

	memcpy(value, str, len);
	value[len] = 0;

	count = len;
#ifdef SSO_STRING
	}
#endif
}

void String::clear() {
#ifdef SSO_STRING
	if (count < SSO_SIZE) {
		count = 0;
	} else {
#endif
	if (value != nullptr) {
		free(value);
		value = nullptr;

		count = 0;
	}
#ifdef SSO_STRING
	}
#endif
}

String String::concat(char ch) const {
	return concat(&ch, 1);
}

String String::concat(int i) const {
	return concat(valueOf(i));
}

String String::concat(const char* str) const {
	return concat(str, strlen(str));
}

String String::concat(const char* str, int len) const {
	String newstr(begin(), count);

	int newlen = count + len;

#ifdef SSO_STRING
	if (newlen < SSO_SIZE) {
		memcpy(newstr.sso + count, str, len);
		newstr.sso[newlen] = 0;

		newstr.count = newlen;
	} else {
		if (newstr.count < SSO_SIZE) {
			char* newvalue = (char*) malloc(newlen + 1);
			memcpy(newvalue, newstr.sso, newstr.count);
			newvalue[newlen] = 0;
			newstr.value = newvalue;
		} else {
#endif
	newstr.value = (char*) realloc(newstr.value, newlen + 1);

#ifdef SSO_STRING
		}
#endif

	memcpy(newstr.value + count, str, len);
	newstr.value[newlen] = 0;

	newstr.count = newlen;
#ifdef SSO_STRING
	}
#endif

	return newstr;
}

String String::concat(const String& str) const {
	return concat(str.begin(), str.count);
}

int String::compareTo(const char* str) const {
	return strcmp(str, begin());
}

int String::compareTo(const String& str) const {
	return compareTo(str.begin());
}

bool String::contains(const char* str) const {
	return indexOf(str) != npos;
}

bool String::contains(const String& str) const {
	return indexOf(str) != npos;
}

int String::indexOf(char ch) const  {
	return indexOf(ch, 0);
}

int String::indexOf(char ch, int fromIndex) const {
	if (fromIndex < 0 || fromIndex >= count)
		return npos;

	const char* position = strchr(begin() + fromIndex, ch);

	if (position != nullptr)
		return position - begin();
	else
		return npos;
}

int String::indexOf(const char* str) const {
	return indexOf(str, 0);
}

int String::indexOf(const char* str, int fromIndex) const {
	if (fromIndex < 0 || fromIndex >= count)
		return npos;

	const char* position = strstr(begin() + fromIndex, str);

	if (position != nullptr)
		return position - begin();
	else
		return npos;
}

int String::indexOf(const String& str) const {
	return indexOf(str, 0);
}

int String::indexOf(const String& str, int fromIndex) const {
	return indexOf(str.toCharArray(), fromIndex);
}

int String::lastIndexOf(char ch) const  {
	return lastIndexOf(ch, 0);
}

int String::lastIndexOf(char ch, int fromIndex) const {
	if (fromIndex < 0 || fromIndex >= count)
		return npos;

	const char* position = strrchr(begin() + fromIndex, ch);

	if (position != nullptr)
		return position - begin();
	else
		return npos;
}

int String::lastIndexOf(const char* str) const {
	return lastIndexOf(str, 0);
}

int String::lastIndexOf(const char* str, int fromIndex) const {
	if (fromIndex < 0 || fromIndex >= count)
		return npos;

	const char* position = strrstr(begin() + fromIndex, count - fromIndex, str, strlen(str));

	if (position != nullptr)
		return position - begin();
	else
		return npos;
}

int String::lastIndexOf(const String& str) const {
	return lastIndexOf(str, 0);
}

int String::lastIndexOf(const String& str, int fromIndex) const {
	if (fromIndex < 0 || fromIndex >= count)
		return npos;

	const char* position = strrstr(begin() + fromIndex, count - fromIndex, str.begin(), str.count);

	if (position != nullptr)
		return position - begin();
	else
		return npos;
}

bool String::beginsWith(const char* str) const {
	return indexOf(str) == 0;
}

bool String::beginsWith(const String& str) const {
	return indexOf(str) == 0;
}

bool String::endsWith(const char* str) const {
	return lastIndexOf(str) == count - (int) strlen(str);
}

bool String::endsWith(const String& str) const {
	return lastIndexOf(str) == count - str.count;
}

uint32 String::hashCode() const {
	return hashCode(begin());
}

uint32 String::hashCode(const String& str) {
	return str.hashCode();
}

String String::subString(int beginIndex) const {
	if (beginIndex < 0 || beginIndex >= count)
		throw ArrayIndexOutOfBoundsException(beginIndex);

	return String(begin() + beginIndex, count - beginIndex);
}

String String::subString(int beginIndex, int endIndex) const {
	if (beginIndex < 0 || beginIndex > count)
		throw ArrayIndexOutOfBoundsException(beginIndex);
	else if (endIndex < 0 || endIndex > count)
		throw ArrayIndexOutOfBoundsException(endIndex);
	else if (beginIndex > endIndex)
		throw ArrayIndexOutOfBoundsException(beginIndex);

	if (beginIndex != endIndex)
		return String(begin() + beginIndex, endIndex - beginIndex);
	else
		return String("");
}

String String::valueOf(int val) {
	char buf[20];

	int written = snprintf(buf, 20, "%i", val);

	E3_ASSERT(written >= 0 && written < 20);

	return String(buf, written);
}

String String::valueOf(long val) {
	return String::valueOf((int64) val);
}

String String::valueOf(uint32 val) {
	char buf[20];

	int written = snprintf(buf, 20, "%u", val);

	E3_ASSERT(written >= 0 && written < 20);

	return String(buf, written);
}

String String::valueOf(int64 val) {
	String str;

	Long::toString(str, val);

	return str;
}

String String::valueOf(uint64 val) {
	String str;

	Long::toString(str, val);

	return str;
}

#ifndef PLATFORM_WIN
String String::valueOf(std::size_t val) {
	String str;

	Long::toString(str, (uint64) val);

	return str;
}
#endif

String String::valueOf(float val) {
	char buf[40];

	int written = snprintf(buf, 40, "%g", val);

	E3_ASSERT(written >= 0 && written < 40);

	return String(buf, written);
}

String String::valueOf(double val) {
	char buf[40];

	int written = snprintf(buf, 40, "%g", val);

	E3_ASSERT(written >= 0 && written < 40);

	return String(buf, written);
}

String String::valueOf(bool val) {
	if (val) {
		return String("true", 4);
	} else {
		return String("false", 5);
	}
}

String String::valueOf(const void* val) {
	char buf[20];

	int written = snprintf(buf, 20, "%p", val);

	E3_ASSERT(written >= 0 && written < 20);

	return String(buf, written);
}

String String::valueOf(const char* val) {
	return String(val);
}

String String::hexvalueOf(int val) {
	char buf[20];

	int written = snprintf(buf, 20, "%x", val);

	E3_ASSERT(written >= 0 && written < 20);

	return String(buf, written);
}

String String::hexvalueOf(long val) {
	return String::hexvalueOf((int64) val);
}

String String::hexvalueOf(uint32 val) {
	char buf[20];

	int written = snprintf(buf, 20, "%x", val);

	E3_ASSERT(written >= 0 && written < 20);

	return String(buf, written);
}

String String::hexvalueOf(int64 val) {
	char buf[32];

#ifdef PLATFORM_WIN
	int written = snprintf(buf, 32, "%I64x", val);
#else
	int written = snprintf(buf, 32, "%llx", val);
#endif

	E3_ASSERT(written >= 0 && written < 32);

	return String(buf, written);
}

String String::hexvalueOf(uint64 val) {
	char buf[32];

#ifdef PLATFORM_WIN
	int written = snprintf(buf, 32, "%I64x", val);
#else
	int written = snprintf(buf, 32, "%llx", val);
#endif

	E3_ASSERT(written >= 0 && written < 32);

	return String(buf, written);
}

String String::replaceFirst(const String& regexString, const String& replacement) const {
	std::regex word_regex(regexString.begin(), std::regex_constants::extended);
	std::match_results<const char*> match;

	if (std::regex_search(begin(), match, word_regex)) {
		int i = match.prefix().length();
		int rlen = match[0].second - match[0].first;

		if (i > 0 && i + rlen < count)
			return subString(0, i) + replacement + subString(i + rlen, count);
		else if (i == 0 && i + rlen < count)
			return replacement + subString(i + rlen, count);
		else if (i == 0 && i + rlen == count)
			return replacement;
		else if (i > 0 && i + rlen == count)
			return subString(0, i) + replacement;
		else
			return regexString;
	} else {
		return *this;
	}
}

int String::regexIndexOf(const String& regex) const {
	std::regex word_regex(regex.begin(), std::regex_constants::extended);
	std::match_results<const char*> match;

	if (std::regex_search(begin(), match, word_regex)) {
		return match.prefix().length();
	}

	return npos;
}

String String::replaceAll(const String& regex, const String& replacement) const {
	StringBuffer str(*this);

	int pos = npos;
	int start = 0;
	int end = npos;

	for (int pos = 0; (pos = str.indexOf(regex, start, end) != npos); ++pos) {
		str.replace(start, end, replacement);

		if (str == replacement)
			break;
	}

	return str.toString();
}

String String::toLowerCase() const {
	String str(begin(), count);

	for (int i = 0; i < count; ++i) {
		char ch = str.begin()[i];

		if (isupper(ch))
			str.begin()[i] = tolower(ch);
	}

	return str;
}

void String::changeToLowerCase() {
	auto ptr = begin();

	for (int i = 0; i < count; ++i) {
		char ch = ptr[i];

		if (isupper(ch))
			ptr[i] = tolower(ch);
	}
}

String String::toUpperCase() const {
	String str(begin(), count);

	for (int i = 0; i < count; ++i) {
		char ch = str.begin()[i];

		if (islower(ch))
			str.begin()[i] = toupper(ch);
	}

	return str;
}

void String::changeToUpperCase() {
	auto ptr = begin();

	for (int i = 0; i < count; ++i) {
		char ch = ptr[i];

		if (islower(ch))
			ptr[i] = toupper(ch);
	}
}

String String::trim() const {
	String str;

	int firstIndex = npos, lastIndex = npos;

	for (int i = 0; i < count; ++i) {
		char ch = begin()[i];

		if (ch != ' ' && ch != '\n' && ch != '\t' && ch != '\r' && ch != '\f') {
			firstIndex = i;
			break;
		}
	}

	if (firstIndex == npos)
		return String("");

	for (int i = count - 1; i >= 0; --i) {
		char ch = begin()[i];

		if (ch != ' ' && ch != '\n' && ch != '\t' && ch != '\r' && ch != '\f') {
			lastIndex = i + 1;
			break;
		}
	}

	return subString(firstIndex, lastIndex);
}

String String::escapeString() const {
	if (isEmpty())
		return String("");

	StringBuffer buf;

	for (int i = 0; i < length(); ++i) {
		char ch = charAt(i);

		switch (ch) {
		case '\0': // Must be escaped for "mysql"
			buf << "\\0";
			break;
		case '\n': // Must be escaped for logs
			buf << "\\n";
			break;
		case '\r':
			buf << "\\r";
			break;
		case '\\':
			buf << "\\\\";
			break;
		case '\"':
			buf << "\\\"";
			break;
		case '\'': // Better safe than sorry
			buf << "\\\'";
			break;
		/*case '\032': // This gives problems on Win32
			buf << "\\Z";
			break;*/
		default:
			buf << ch;
			break;
		}
	}

	return buf.toString();
}

String& String::operator=(const char* str) {
	clear();

	create(str, strlen(str));

	return *this;
}

String& String::operator=(const String& str) {
	if (begin() == str.begin())
		return *this;

	clear();

	create(str.begin(), str.count);

	return *this;
}

String& String::operator=(const std::string& str) {
	clear();

	create(str.c_str(), str.size());

	return *this;
}

String& String::operator+=(const char* str) {
	int len = strlen(str);
	int newlen = count + len;

#ifdef SSO_STRING
	if (newlen < SSO_SIZE) {
		memcpy(sso + count, str, len);
		sso[newlen] = 0;

		count = newlen;
	} else {
		if (count < SSO_SIZE) {
			char* newvalue = (char*) malloc(newlen + 1);
			memcpy(newvalue, sso, count);
			newvalue[newlen] = 0;
			value = newvalue;
		} else {
#endif
			value = (char*) realloc(value, newlen + 1);
#ifdef SSO_STRING
		}
#endif

	memcpy(value + count, str, len);
	value[newlen] = 0;

	count = newlen;
#ifdef SSO_STRING
	}
#endif

	return *this;
}

String& String::operator+=(char ch) {
	int newlen = count + 1;

#ifdef SSO_STRING
	if (newlen < SSO_SIZE) {
		sso[newlen - 1] = ch;
		sso[newlen] = 0;

		count = newlen;
	} else {
		if (count < SSO_SIZE) {
			char* newvalue = (char*) malloc(newlen + 1);
			memcpy(newvalue, sso, count);
			newvalue[newlen] = 0;
			value = newvalue;
		} else {
#endif
	value = (char*) realloc(value, newlen + 1);
#ifdef SSO_STRING
		}
#endif
	value[newlen - 1] = ch;
	value[newlen] = 0;

	count = newlen;
#ifdef SSO_STRING
	}
#endif

	return *this;
}

String& String::operator+=(const String& str) {
	int newlen = count + str.count;

#ifdef SSO_STRING
	if (newlen < SSO_SIZE) {
		memcpy(sso + count, str.toCharArray(), str.count);
		sso[newlen] = 0;

		count = newlen;
	} else {
		if (count < SSO_SIZE) {
			char* newvalue = (char*) malloc(newlen + 1);
			memcpy(newvalue, sso, count);
			newvalue[newlen] = 0;
			value = newvalue;
		} else {
#endif
	value = (char*) realloc(value, newlen + 1);
#ifdef SSO_STRING
		}
#endif
	memcpy(value + count, str.toCharArray(), str.count);
	value[newlen] = 0;

	count = newlen;

#ifdef SSO_STRING
	}
#endif

	return *this;
}

char& String::operator[](int i) {
	if (i < 0 || i >= count)
		throw ArrayIndexOutOfBoundsException(i);

	return begin()[i];
}

const char& String::operator[](int i) const {
	if (i < 0 || i >= count)
		throw ArrayIndexOutOfBoundsException(i);

	return begin()[i];
}

char String::charAt(int index) const {
	if (index < 0 || index >= count)
		throw ArrayIndexOutOfBoundsException(index);

	return begin()[index];
}

bool String::toBinaryStream(ObjectOutputStream* stream) {
	E3_ASSERT(count <= UINT16_MAX && "String length exceeded 65k in toBinaryStream");

	stream->writeShort(count);
	stream->writeStream(begin(), count);

	return true;
}

bool String::parseFromBinaryStream(ObjectInputStream* stream) {
	uint16 len = stream->readShort();
	auto buffer = stream->getBuffer() + stream->getOffset();

	stream->shiftOffset(len);

	clear();
	create(buffer, len);

	return true;
}

bool String::toString(String& str) {
	StringBuffer buffer;
	buffer << '"' << escapeString() << '"';

	buffer.toString(str);

	return true;
}

String& String::operator=(String&& str) {
	if (this == &str)
		return *this;

	clear();

	count = str.count;

#ifdef SSO_STRING
	if (count < SSO_SIZE) {
		memcpy(sso, str.sso, count);
		sso[count] = 0;
	} else {
		value = str.value;
		str.value = nullptr;
		str.count = 0;
	}
#else
	value = str.value;
	str.value = nullptr;
	str.count = 0;
#endif

	return *this;
}

bool String::parseFromString(const String& str, int version) {
	if (str.length() < 2)
		return false;

	StringBuffer buffer;
	bool skip = false;

	for (int i = 1; i < str.length() - 1; ++i) {
		if (skip) {
			skip = false;
			buffer << str.charAt(i);
			continue;
		} else if (str.charAt(i) == '\\') {
			skip = true;
			continue;
		} else
			buffer << str.charAt(i);
	}

	buffer.toString(*this);

	return true;
}

const char* String::strrstr(const char* s, int slen, const char* t, int tlen) {
	int i, j;

	for (i = slen; i >= tlen; i--) {
		for (j = 0; j < tlen && s[i - tlen + j] == t[j]; j++) ;

		if (j == tlen)
			return (s + i - tlen);
	}

	return nullptr;
}

