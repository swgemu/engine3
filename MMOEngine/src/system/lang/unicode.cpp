/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../platform.h"

#include "unicode.h"

#include "ArrayIndexOutOfBoundsException.h"

unicode::unicode() {
	create("", 0);
}

unicode::unicode(const char* ascii) {
	int len = strlen(ascii);
	create(ascii, len);
}

unicode::unicode(const string& ascii) {
	create(ascii.c_str(), ascii.size());
}

unicode::unicode(const char* ascii, int len) {
	create(ascii, len);
}

unicode::unicode(const unicode& str) {
	ustring = new wchar_t[str.count + 1];
	count = str.count;
	//wcscpy(ustring, str.ustring);
	copy(ustring, str.ustring);
}

unicode::~unicode() {
	delete[] ustring;
}

void unicode::create(const char* ascii, int len) {
	//mbstate_t state;
	//memset(&state, '\0', sizeof (state));

	ustring = new wchar_t[len + 1];
	//mbsrtowcs(ustring, &ascii, len, &state);
	asciitowide(ustring, ascii, len);

	ustring[count = len] = 0;
}

void unicode::asciitowide(wchar_t* unicode, const char* ascii, int len) {
	for (int i = 0; i < len; ++i)
		unicode[i] = ascii[i];
}

void unicode::copy(wchar_t* dest, const wchar_t* src) {
	for (int i = 0; true; ++i) {
		wchar_t chr = src[i];

		dest[i] = chr;

		if (chr == 0)
			break;
	}
}

void unicode::copy(wchar_t* dest, const wchar_t* src, int len) {
	for (int i = 0; i < len; ++i) {
		wchar_t chr = src[i];

		dest[i] = chr;

		if (chr == 0)
			break;
	}
}

unicode& unicode::operator=(const unicode& str) {
	if (this != &str) {
		delete[] ustring;

		count = str.count;
		ustring = new wchar_t[count + 1];

		//wcscpy(ustring, str.ustring);
		copy(ustring, str.ustring);
	}

	return *this;
}

unicode& unicode::operator=(const string& ascii) {
	clear();
	append(ascii);

	return *this;
}

unicode& unicode::operator=(const char* ascii) {
	clear();
	append(ascii);

	return *this;
}

unicode& unicode::operator+(const string& ascii) {
	append(ascii);
	return *this;
}

unicode& unicode::operator+(const char* ascii) {
	append(ascii);

	return *this;
}

unicode& unicode::operator+(const unicode& str) {
	append(str);
	return *this;
}

char unicode::operator[](int index) const {
	return (char) ustring[index];
}

void unicode::append(const string& ascii) {
	append(ascii.c_str(), ascii.size());
}

void unicode::append(const unicode& uni) {
	append((wchar_t*)uni.u_str(), uni.size());
}

void unicode::append(const char* ascii) {
	int len = strlen(ascii);
	append(ascii, len);
}

void unicode::append(const char* ascii, int len) {
	//mbstate_t state;
  	//memset (&state, '\0', sizeof (state));

  	int ncount = count + len;
	wchar_t *nustring = new wchar_t[ncount + 1];

	//wmemcpy(nustring, ustring, count);
	copy(nustring, ustring, count);
	delete[] ustring;

	//mbsrtowcs(nustring + count, &ascii, len, &state);
	asciitowide(nustring + count, ascii, len);

	ustring = nustring;
	ustring[count = ncount] = 0;
}

void unicode::append(const wchar_t* str, int len) {
	//mbstate_t state;
    //memset (&state, '\0', sizeof (state));

  	int ncount = count + len;
	wchar_t *nustring = new wchar_t[ncount + 1];

	//wmemcpy(nustring, ustring, count);
	copy(nustring, ustring, count);
	delete[] ustring;

	//wmemcpy(nustring + count, str, len);
	copy(nustring + count, str, len);

	ustring = nustring;
	ustring[count = ncount] = 0;
}

int unicode::indexOf(wchar_t chr) const {
	for (int i = 0; i < count; ++i) {
		if (ustring[i] == chr)
			return i;
	}

	return -1;

	/*wchar_t* pos = wcschr(ustring, chr);

	if (pos != NULL)
		return (int) (pos - ustring);
	else
		return -1;*/
}

int unicode::find(const unicode& str, int startPos) const {
	if (str.size() == 0)
		return -1;

	for (int i = startPos; i <= count - str.size(); ++i) {
		if (!memcmp(ustring + i, str.u_str(), str.size() * sizeof(wchar_t)))
			return i;
	}

	return -1;
}

unicode unicode::substring(int beg, int end) const {
	unicode nstr;
	nstr.append(ustring + beg, end - beg);

	return nstr;
}

unicode unicode::substr(int beg, int length) const {
	unicode nstr;

	if (length == 0)
		return nstr;

	nstr.append(ustring + beg, length);

	return nstr;
}

void unicode::clear() {
	delete[] ustring;

	create("", 0);
}

const char* unicode::u_str() const {
	return (char*) ustring;
}

string unicode::c_str() const {
	string ascii = "";

	for (int i = 0; i < count; ++i)
		ascii.append((char*) (ustring+i));

	return ascii;
}

void unicode::c_str(string& ascii) const {
	ascii.clear();

	for (int i = 0; i < count; ++i)
		ascii.append((char*) (ustring+i));
}
