/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "system/lang/Character.h"

#include "File.h"

PrintStream::PrintStream(StreamType type) : streamFlags(SF_none), outStream(type) {
}

PrintStream::PrintStream(File& file) : streamFlags(SF_none) {
	file.setText();

	file.setWriteable();

	outStream = file.getDescriptor();
}

void PrintStream::print(char ch) {
	if (doUpperCase()) {
		ch = islower(ch) ? toupper(ch) : ch;
	}

	fprintf(outStream, "%c", ch);
}

void PrintStream::print(int val) {
	if (doHex())
		fprintf(outStream, upf("%x", "%X"), val);
	else
		fprintf(outStream, "%i", val);
}

void PrintStream::print(uint32 val) {
	if (doHex())
		fprintf(outStream, upf("%x", "%X"), val);
	else
		fprintf(outStream, "%u", val);
}

void PrintStream::print(long val) {
	if (doHex())
		fprintf(outStream, upf("%lx", "%lX"), val);
	else
		fprintf(outStream, "%ld", val);
}

void PrintStream::flush() {
	::fflush(outStream);
}

void PrintStream::print(int64 val) {
#ifndef PLATFORM_WIN
	if (doHex())
		fprintf(outStream, upf("%llx", "%llX"), val);
	else
		fprintf(outStream, "%lld", val);
#else
	if (doHex())
		fprintf(outStream, "%I64dx", val);
	else
		fprintf(outStream, "%I64d", val);
#endif
}

void PrintStream::print(uint64 val) {
#ifndef PLATFORM_WIN
	if (doHex())
		fprintf(outStream, upf("%llx", "%llX"), val);
	else
		fprintf(outStream, "%llu", val);
#else
	if (doHex())
		fprintf(outStream, "%I64ux", val);
	else
		fprintf(outStream, "%I64u", val);
#endif
}

void PrintStream::print(float val) {
	if (doHex())
		fprintf(outStream, upf("%a", "%A"), val);
	else
		fprintf(outStream, upf("%f", "%F"), val);
}

void PrintStream::print(void* val) {
	fprintf(outStream, "%p", val);
}

void PrintStream::print(const char* str) {
	if (doUpperCase()) {
		String upper(str);
		upper.changeToUpperCase();

		fprintf(outStream, "%s", upper.toCharArray());
	} else {
		fprintf(outStream, "%s", str);
	}
}

void PrintStream::print(const char* str, int length) {
	if (doUpperCase()) {
		String upper(str, length);
		upper.changeToUpperCase();

		fprintf(outStream, "%.*s", upper.length(), upper.toCharArray());
	} else {
		fprintf(outStream, "%.*s", length, str);
	}
}

void PrintStream::print(const String& str) {
	if (doUpperCase()) {
		fprintf(outStream, "%s", str.toUpperCase().toCharArray());
	} else {
		fprintf(outStream, "%s", str.toCharArray());
	}
}

void PrintStream::print(const UnicodeString& val) {
	auto str = val.toString();

	if (doUpperCase()) {
		str.changeToUpperCase();
	}

	fprintf(outStream, "%s", str.toCharArray());
}

void PrintStream::println(char ch) {
	print(ch);

	fprintf(outStream, "\n");
}

void PrintStream::println(int val) {
	print(val);

	fprintf(outStream, "\n");
}

void PrintStream::println(uint32 val) {
	print(val);

	fprintf(outStream, "\n");
}

void PrintStream::println(long val) {
	print(val);

	fprintf(outStream, "\n");
}

void PrintStream::println(int64 val) {
	print(val);

	fprintf(outStream, "\n");
}

void PrintStream::println(uint64 val) {
	print(val);

	fprintf(outStream, "\n");
}

void PrintStream::println(float val) {
	print(val);

	fprintf(outStream, "\n");
}

void PrintStream::println(void* val) {
	print(val);

	fprintf(outStream, "\n");
}

void PrintStream::println(const char* str) {
	print(str);

	fprintf(outStream, "\n");
}

void PrintStream::println(const char* str, int length) {
	print(str, length);

	fprintf(outStream, "\n");
}

void PrintStream::println(const String& str) {
	print(str);

	fprintf(outStream, "\n");
}

void PrintStream::println(const UnicodeString& val) {
	print(val);

	fprintf(outStream, "\n");
}

PrintStream& PrintStream::operator<<(char ch) {
	print(ch);

	return *this;
}

PrintStream& PrintStream::operator<<(int val) {
	print(val);

	return *this;
}

PrintStream& PrintStream::operator<<(uint32 val) {
	print(val);

	return *this;
}

PrintStream& PrintStream::operator<<(long val) {
	print(val);

	return *this;
}

PrintStream& PrintStream::operator<<(int64 val) {
	print(val);

	return *this;
}

PrintStream& PrintStream::operator<<(uint64 val) {
	print(val);

	return *this;
}

PrintStream& PrintStream::operator<<(unsigned long val) {
	print((uint64)val);

	return *this;
}

PrintStream& PrintStream::operator<<(float val) {
	print(val);

	return *this;
}

PrintStream& PrintStream::operator<<(void* val) {
	print(val);

	return *this;
}

PrintStream& PrintStream::operator<<(const char* str) {
	print(str);

	return *this;
}

PrintStream& PrintStream::operator<<(const StringBuffer& msg) {
	print(msg.getBuffer(), msg.length());

	return *this;
}

PrintStream& PrintStream::operator<<(const String& str) {
	print(str);

	return *this;
}

PrintStream& PrintStream::operator<<(const UnicodeString& str) {
	print(str);

	return *this;
}

PrintStream& PrintStream::operator<< (const StreamFlags flags) {
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
