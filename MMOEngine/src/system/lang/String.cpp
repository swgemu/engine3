/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "String.h"
#include "StringBuffer.h"

#include "Long.h"

#include "ArrayIndexOutOfBoundsException.h"
#include "IllegalArgumentException.h"
#include "NumberFormatException.h"

static const unsigned int crctable[256] = {
    0x0000000,
    0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B,
    0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6,
    0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
    0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC,
    0x5BD4B01B, 0x569796C2, 0x52568B75, 0x6A1936C8, 0x6ED82B7F,
    0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A,
    0x745E66CD, 0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
    0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58,
    0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033,
    0xA4AD16EA, 0xA06C0B5D, 0xD4326D90, 0xD0F37027, 0xDDB056FE,
    0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
    0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4,
    0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 0x34867077, 0x30476DC0,
    0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5,
    0x2AC12072, 0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16,
    0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA, 0x7897AB07,
    0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C,
    0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1,
    0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
    0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B,
    0xBB60ADFC, 0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698,
    0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D,
    0x94EA7B2A, 0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E,
    0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2, 0xC6BCF05F,
    0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34,
    0xDC3ABDED, 0xD8FBA05A, 0x690CE0EE, 0x6DCDFD59, 0x608EDB80,
    0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
    0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A,
    0x58C1663D, 0x558240E4, 0x51435D53, 0x251D3B9E, 0x21DC2629,
    0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C,
    0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
    0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 0xF12F560E,
    0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65,
    0xEBA91BBC, 0xEF68060B, 0xD727BBB6, 0xD3E6A601, 0xDEA580D8,
    0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
    0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2,
    0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71,
    0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74,
    0x857130C3, 0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640,
    0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 0x7B827D21,
    0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A,
    0x61043093, 0x65C52D24, 0x119B4BE9, 0x155A565E, 0x18197087,
    0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
    0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D,
    0x2056CD3A, 0x2D15EBE3, 0x29D4F654, 0xC5A92679, 0xC1683BCE,
    0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB,
    0xDBEE767C, 0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18,
    0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4, 0x89B8FD09,
    0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662,
    0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF,
    0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4,
};

String::String() {
	create("", 0);
}

String::String(char* str) {
	if (str == NULL)
		throw IllegalArgumentException();

	create(str, strlen(str));
}

String::String(const char* str) {
	if (str == NULL)
		throw IllegalArgumentException();

	create(str, strlen(str));
}

String::String(const char* str, int len) {
	if (str == NULL)
		throw IllegalArgumentException();

	create(str, len);
}

String::String(const String& str) {
	create(str.value, str.count);
}

String::~String() {
	destroy();
}

void String::create(const char* str, int len) {
	value = (char*) malloc(len + 1);

	memcpy(value, str, len);
	value[len] = 0;

	count = len;
}

void String::destroy() {
	if (value != NULL) {
		free(value);
		value = NULL;

		count = 0;
	}
}

String String::concat(const char* str) const {
	return concat(str, strlen(str));
}

String String::concat(char ch) const {
	return concat(&ch, 1);
}

String String::concat(const char* str, int len) const {
	int newlen = count + len;

	String newstr;
	newstr.value = (char*) malloc(newlen + 1);

	memcpy(newstr.value, value, count);

	memcpy(newstr.value + count, str, len);
	newstr.value[newlen] = 0;

	newstr.count = newlen;

	return newstr;
}

String String::concat(const String& str) const {
	return concat(str.value, str.count);
}

int String::compareTo(const char* str) const {
	return strcmp(value, str);
}

int String::compareTo(const String& str) const {
	return compareTo(str.value);
}

int String::indexOf(char ch) const  {
	return indexOf(ch, 0);
}

int String::indexOf(char ch, int fromIndex) const {
	char* position = strchr(value + fromIndex, ch);

	if (position != NULL)
		return position - value;
	else
		return -1;
}

int String::indexOf(const String& str) const {
	return indexOf(str, 0);
}

int String::indexOf(const String& str, int fromIndex) const {
	char* position = strstr(value + fromIndex, str);

	if (position != NULL)
		return position - value;
	else
		return -1;
}

int String::lastIndexOf(char ch) const  {
	return lastIndexOf(ch, 0);
}

int String::lastIndexOf(char ch, int fromIndex) const {
	char* position = strrchr(value + fromIndex, ch);

	if (position != NULL)
		return position - value;
	else
		return -1;
}

int String::lastIndexOf(const String& str) const {
	return lastIndexOf(str, 0);
}

int String::lastIndexOf(const String& str, int fromIndex) const {
	/*char* position = strrstr(value + fromIndex, count - fromIndex, str.value, str.count);

	if (position != NULL)
		return position - value;
	else*/
		return -1;
}

uint32 String::hashCode() const {
	uint32 CRC = 0xFFFFFFFF;

	for (int counter = 0; counter < count; counter++)
  		CRC = crctable[value[counter] ^ (CRC >> 24)] ^ (CRC << 8);

	return ~CRC;
}

String String::subString(int beginIndex) const {
	if (beginIndex < 0 || beginIndex >= count)
		throw ArrayIndexOutOfBoundsException(beginIndex);

	return String(value + beginIndex, count - beginIndex);
}

String String::subString(int beginIndex, int endIndex) const {
	if (beginIndex < 0 || beginIndex > count)
		throw ArrayIndexOutOfBoundsException(beginIndex);
	else if (endIndex < 0 || endIndex > count)
		throw ArrayIndexOutOfBoundsException(endIndex);
	else if (beginIndex > endIndex)
		throw ArrayIndexOutOfBoundsException(beginIndex);

	if (beginIndex != endIndex)
		return String(value + beginIndex, endIndex - beginIndex);
	else
		return String("");
}

String String::valueOf(int val) {
	char buf[10];

	sprintf(buf, "%d", val);

	return String(buf);
}

String String::valueOf(uint32 val) {
	char buf[10];

	sprintf(buf, "%u", val);

	return String(buf);
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

String String::valueOf(float val) {
	char buf[20];

	sprintf(buf, "%g", val);

	return String(buf);
}

String String::valueOf(double val) {
	char buf[20];

	sprintf(buf, "%g", (float) val);

	return String(buf);
}

String String::valueOf(void* val) {
	char buf[20];

	sprintf(buf, "%p", val);

	return String(buf);
}

String String::hexvalueOf(int val) {
	char buf[20];

	sprintf(buf, "%x", val);

	return String(buf);
}

String String::replaceFirst(const String& regex, const String& replacement) const {
	int rlen = regex.count;

	int i = indexOf(regex);

	if (i != -1) {
		if (i > 0 && i + rlen < count)
			return subString(0, i) + replacement + subString(i + rlen, count);
		else if (i == 0 && i + rlen < count)
			return replacement + subString(i + rlen, count);
		else if (i > 0 && i + rlen == count)
			return subString(0, i) + replacement;
		else
			return regex;
	} else
		return *this;
}

String String::replaceAll(const String& regex, const String& replacement) const {
	StringBuffer str(*this);

	for (int pos = 0; (pos = str.indexOf(regex, pos)) != -1; ++pos)
		str.replace(pos, pos + regex.length(), replacement);

	return str.toString();
}

String String::toLowerCase() const {
	String str(value);

	for (int i = 0; i < count; ++i) {
		char ch = value[i];

		if (isupper(ch))
			str.value[i] = tolower(ch);
	}

	return str;
}

String String::toUpperCase() const {
	String str(value);

	for (int i = 0; i < count; ++i) {
		char ch = value[i];

		if (islower(ch))
			str.value[i] = toupper(ch);
	}

	return str;
}

String String::trim() const {
	String str;

	int firstIndex = -1, lastIndex = -1;

	for (int i = 0; i < count; ++i) {
		char ch = value[i];

		if (ch != ' ' && ch != '\n' && ch != '\t' && ch != '\r' && ch != '\f') {
			firstIndex = i;
			break;
		}
	}

	if (firstIndex == -1)
		return String("");

	for (int i = count - 1; i >= 0; --i) {
		char ch = value[i];

		if (ch != ' ' && ch != '\n' && ch != '\t' && ch != '\r' && ch != '\f') {
			lastIndex = i + 1;
			break;
		}
	}

	return subString(firstIndex, lastIndex);
}

String& String::operator=(const char* str) {
	if (value != NULL)
		free(value);

	create(str, strlen(str));

	return *this;
}

String& String::operator=(const String& str) {
	if (value == str.value)
		return *this;

	if (value != NULL)
		free(value);

	create(str.value, str.count);

	return *this;
}

String& String::operator+=(const char* str) {
	int len = strlen(str);
	int newlen = count + len;

	value = (char*) realloc(value, newlen + 1);

	memcpy(value + count, str, len);
	value[newlen] = 0;

	count = newlen;

	return *this;
}

String& String::operator+=(char ch) {
	int newlen = count + 1;

	value = (char*) realloc(value, newlen + 1);

	value[newlen - 1] = ch;
	value[newlen] = 0;

	count = newlen;

	return *this;
}

String& String::operator+=(const String& str) {
	int newlen = count + str.count;

	value = (char*) realloc(value, newlen + 1);

	memcpy(value + count, str, str.count);
	value[newlen] = 0;

	count = newlen;

	return *this;
}

char String::charAt(int index) const {
	if (index < 0 || index >= count)
		throw ArrayIndexOutOfBoundsException(index);

	return value[index];
}

bool operator==(char ch, const String& str2) {
	return String(&ch, 1) == str2;
}

bool operator==(const String& str, char ch) {
	return String(&ch, 1) == str;
}

bool operator==(char* str1, const String& str2) {
	return String(str1) == str2;
}

bool operator==(const char* str1, const String& str2) {
	return String(str1) == str2;
}

bool operator!=(char ch, const String& str2) {
	return String(&ch, 1) != str2;
}

bool operator!=(const char* str1, const String& str2) {
	return String(str1) != str2;
}

bool operator!=(char* str1, const String& str2) {
	return String(str1) != str2;
}

bool operator!=(const String& str, char ch) {
	return String(&ch, 1) != str;
}

String operator+(const String& str1, const String& str2) {
	return str1.concat(str2);
}

String operator+(const char* str1, const String& str2) {
	return String(str1).concat(str2);
}

String operator+(const String& str1, const char* str2) {
	return str1.concat(str2);
}

String operator+(const String& str1, char ch) {
	return str1.concat(ch);
}

String operator+(char ch, const String& str2) {
	return String(&ch, 1).concat(str2);
}

char* String::strrstr(const char* s, int slen, const char* t, int tlen) {
	int i, j;

	for (i = slen; i >= tlen; i--) {
		for (j = 0; j < tlen && s[i - tlen + j] == t[j]; j++);

		if (j == tlen)
			return (char *) (s + i - tlen);
	}

	return NULL;
}
