/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include <cstdio>

#include "system/lang/Character.h"

#include "File.h"
#include "TextStream.h"

namespace TextStreamDetail {
	int fprintf(FILE* str, const char* format, ...) {
		va_list args;
		va_start(args, format);

		int res = ::vfprintf(str, format, args);

		va_end(args);

		if (res < 0) {
			throw Exception("fprintf failed: " + String::valueOf(res));
		}

		return res;
	}
}

TextStream::TextStream(StreamType type) : streamFlags(SF_none), outStream(type) {
}

TextStream::TextStream(File& file) : streamFlags(SF_none) {
	file.setText();

	file.setWriteable();

	outStream = file.getDescriptor();
}

void TextStream::print(char ch) {
	if (doUpperCase()) {
		ch = islower(ch) ? toupper(ch) : ch;
	}

	TextStreamDetail::fprintf(outStream, "%c", ch);
}

void TextStream::print(int val) {
	if (doHex())
		TextStreamDetail::fprintf(outStream, upf("%x", "%X"), val);
	else
		TextStreamDetail::fprintf(outStream, "%i", val);
}

void TextStream::print(uint32 val) {
	if (doHex())
		TextStreamDetail::fprintf(outStream, upf("%x", "%X"), val);
	else
		TextStreamDetail::fprintf(outStream, "%u", val);
}

void TextStream::print(long val) {
	if (doHex())
		TextStreamDetail::fprintf(outStream, upf("%lx", "%lX"), val);
	else
		TextStreamDetail::fprintf(outStream, "%ld", val);
}

void TextStream::flush() {
	::fflush(outStream);
}

void TextStream::print(int64 val) {
#ifndef PLATFORM_WIN
	if (doHex())
		TextStreamDetail::fprintf(outStream, upf("%llx", "%llX"), val);
	else
		TextStreamDetail::fprintf(outStream, "%lld", val);
#else
	if (doHex())
		TextStreamDetail::fprintf(outStream, "%I64dx", val);
	else
		TextStreamDetail::fprintf(outStream, "%I64d", val);
#endif
}

void TextStream::print(uint64 val) {
#ifndef PLATFORM_WIN
	if (doHex())
		TextStreamDetail::fprintf(outStream, upf("%llx", "%llX"), val);
	else
		TextStreamDetail::fprintf(outStream, "%llu", val);
#else
	if (doHex())
		TextStreamDetail::fprintf(outStream, "%I64ux", val);
	else
		TextStreamDetail::fprintf(outStream, "%I64u", val);
#endif
}

void TextStream::print(float val) {
	if (doHex())
		TextStreamDetail::fprintf(outStream, upf("%a", "%A"), val);
	else
		TextStreamDetail::fprintf(outStream, upf("%f", "%F"), val);
}

void TextStream::print(void* val) {
	TextStreamDetail::fprintf(outStream, "%p", val);
}

void TextStream::print(const char* str) {
	if (doUpperCase()) {
		String upper(str);
		upper.changeToUpperCase();

		TextStreamDetail::fprintf(outStream, "%s", upper.toCharArray());
	} else {
		TextStreamDetail::fprintf(outStream, "%s", str);
	}
}

void TextStream::print(const char* str, int length) {
	if (doUpperCase()) {
		String upper(str, length);
		upper.changeToUpperCase();

		TextStreamDetail::fprintf(outStream, "%.*s", upper.length(), upper.toCharArray());
	} else {
		TextStreamDetail::fprintf(outStream, "%.*s", length, str);
	}
}

void TextStream::print(const String& str) {
	if (doUpperCase()) {
		TextStreamDetail::fprintf(outStream, "%s", str.toUpperCase().toCharArray());
	} else {
		TextStreamDetail::fprintf(outStream, "%s", str.toCharArray());
	}
}

void TextStream::print(const UnicodeString& val) {
	auto str = val.toString();

	if (doUpperCase()) {
		str.changeToUpperCase();
	}

	TextStreamDetail::fprintf(outStream, "%s", str.toCharArray());
}

void TextStream::println(char ch) {
	print(ch);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(int val) {
	print(val);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(uint32 val) {
	print(val);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(long val) {
	print(val);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(int64 val) {
	print(val);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(uint64 val) {
	print(val);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(float val) {
	print(val);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(void* val) {
	print(val);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(const char* str) {
	print(str);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(const char* str, int length) {
	print(str, length);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(const String& str) {
	print(str);

	TextStreamDetail::fprintf(outStream, "\n");
}

void TextStream::println(const UnicodeString& val) {
	print(val);

	TextStreamDetail::fprintf(outStream, "\n");
}

TextStream& TextStream::operator<<(char ch) {
	print(ch);

	return *this;
}

TextStream& TextStream::operator<<(int val) {
	print(val);

	return *this;
}

TextStream& TextStream::operator<<(uint32 val) {
	print(val);

	return *this;
}

TextStream& TextStream::operator<<(long val) {
	print(val);

	return *this;
}

TextStream& TextStream::operator<<(int64 val) {
	print(val);

	return *this;
}

TextStream& TextStream::operator<<(uint64 val) {
	print(val);

	return *this;
}

TextStream& TextStream::operator<<(unsigned long val) {
	print((uint64)val);

	return *this;
}

TextStream& TextStream::operator<<(float val) {
	print(val);

	return *this;
}

TextStream& TextStream::operator<<(void* val) {
	print(val);

	return *this;
}

TextStream& TextStream::operator<<(const char* str) {
	print(str);

	return *this;
}

TextStream& TextStream::operator<<(const StringBuffer& msg) {
	print(msg.getBuffer(), msg.length());

	return *this;
}

TextStream& TextStream::operator<<(const String& str) {
	print(str);

	return *this;
}

TextStream& TextStream::operator<<(const UnicodeString& str) {
	print(str);

	return *this;
}

TextStream& TextStream::operator<< (const StreamFlags flags) {
	switch (flags) {
	case sys::lang::dec:
		streamFlags = (StreamFlags) ((uint32) streamFlags & ~(uint32) hex);
		break;
	case sys::lang::endl:
		println("");
		break;
	case sys::lang::nouppercase:
		streamFlags = (StreamFlags) ((uint32) streamFlags & ~(uint32) uppercase);
		break;
	case sys::lang::flush:
		flush();
		break;
	default:
		streamFlags = (StreamFlags) ((uint32) streamFlags | (uint32) flags);
		break;
	}

	return *this;
}
