/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "system/lang/Character.h"

PrintStream::PrintStream() {
	streamFlags = SF_none;
	setbuf(stdout, 0);
}


void PrintStream::print(char ch) {
	if (doUpperCase()) {
		ch = islower(ch) ? toupper(ch) : ch;
	}

	printf("%c", ch);
}

void PrintStream::print(int val) {
	if (doHex())
		printf(upf("%x", "%X"), val);
	else
		printf("%i", val);
}

void PrintStream::print(uint32 val) {
	if (doHex())
		printf(upf("%x", "%X"), val);
	else
		printf("%u", val);
}

void PrintStream::print(long val) {
	if (doHex())
		printf(upf("%lx", "%lX"), val);
	else
		printf("%ld", val);
}

void PrintStream::flush() {
	::fflush(stdout);
}

void PrintStream::print(int64 val) {
#ifndef PLATFORM_WIN
	if (doHex())
		printf(upf("%lldx", "%lldX"), val);
	else
		printf("%lld", val);
#else
	if (doHex())
		printf("%I64dx", val);
	else
		printf("%I64d", val);
#endif
}

void PrintStream::print(uint64 val) {
#ifndef PLATFORM_WIN
	if (doHex())
		printf(upf("%llx", "%llX"), val);
	else
		printf("%llu", val);
#else
	if (doHex())
		printf("%I64ux", val);
	else
		printf("%I64u", val);
#endif
}

void PrintStream::print(float val) {
	if (doHex())
		printf(upf("%a", "%A"), val);
	else
		printf(upf("%f", "%F"), val);
}

void PrintStream::print(void* val) {
	printf("%p", val);
}

void PrintStream::print(const char* str) {
	if (doUpperCase()) {
		String upper(str);
		upper.changeToUpperCase();

		printf("%s", upper.toCharArray());
	} else {
		printf("%s", str);
	}
}

void PrintStream::print(const char* str, int length) {
	if (doUpperCase()) {
		String upper(str, length);
		upper.changeToUpperCase();

		printf("%.*s", upper.length(), upper.toCharArray());
	} else {
		printf("%.*s", length, str);
	}
}

void PrintStream::print(const String& str) {
	if (doUpperCase()) {
		printf("%s", str.toUpperCase().toCharArray());
	} else {
		printf("%s", str.toCharArray());
	}
}

void PrintStream::print(const UnicodeString& val) {
	auto str = val.toString();

	if (doUpperCase()) {
		str.changeToUpperCase();
	}

	printf("%s", str.toCharArray());
}

void PrintStream::println(char ch) {
	print(ch);

	printf("\n");
}

void PrintStream::println(int val) {
	print(val);

	printf("\n");
}

void PrintStream::println(uint32 val) {
	print(val);

	printf("\n");
}

void PrintStream::println(long val) {
	print(val);

	printf("\n");
}

void PrintStream::println(int64 val) {
	print(val);

	printf("\n");
}

void PrintStream::println(uint64 val) {
	print(val);

	printf("\n");
}

void PrintStream::println(float val) {
	print(val);

	printf("\n");
}

void PrintStream::println(void* val) {
	print(val);

	printf("\n");
}

void PrintStream::println(const char* str) {
	print(str);

	printf("\n");
}

void PrintStream::println(const char* str, int length) {
	print(str, length);

	printf("\n");
}

void PrintStream::println(const String& str) {
	print(str);

	printf("\n");
}

void PrintStream::println(const UnicodeString& val) {
	print(val);

	printf("\n");
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
	case sys::lang::lowercase:
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
