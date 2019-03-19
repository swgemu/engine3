#include "system/lang/Character.h"

PrintStream::PrintStream() {
	streamFlags = SF_none;
	setbuf(stdout, 0);
}

void PrintStream::print(char ch) {
	printf("%c", ch);
}

void PrintStream::print(int val) {
	if (doHex())
		printf("%x", val);
	else
		printf("%i", val);
}

void PrintStream::print(uint32 val) {
	if (doHex())
		printf("%x", val);
	else
		printf("%u", val);
}

void PrintStream::print(long val) {
	if (doHex())
		printf("%ldx", val);
	else
		printf("%ld", val);
}

void PrintStream::print(int64 val) {
#ifndef PLATFORM_WIN
	if (doHex())
		printf("%lldx", val);
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
		printf("%llux", val);
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
	printf("%f", val);
}

void PrintStream::print(void* val) {
	printf("%p", val);
}

void PrintStream::print(const char* str) {
	printf("%s", str);
}

void PrintStream::print(const char* str, int length) {
	printf("%.*s", length, str);
}

void PrintStream::print(const String& str) {
	printf("%s", str.toCharArray());
}

void PrintStream::println(char ch) {
	printf("%c\n", ch);
}

void PrintStream::println(int val) {
	if (doHex())
		printf("%ih", val);
	else
		printf("%i", val);
}

void PrintStream::println(uint32 val) {
	if (doHex())
		printf("%uh\n", val);
	else
		printf("%u\n", val);
}

void PrintStream::println(long val) {
	if (doHex())
		printf("%ldh\n", val);
	else
		printf("%ld\n", val);
}

void PrintStream::println(int64 val) {
#ifndef PLATFORM_WIN
	if (doHex())
		printf("%lldh", val);
	else
		printf("%lld", val);
#else
	if (doHex())
		printf("%I64dh", val);
	else
		printf("%I64d", val);
#endif
}

void PrintStream::println(uint64 val) {
#ifndef PLATFORM_WIN
	if (doHex())
		printf("%lluh\n", val);
	else
		printf("%llu\n", val);
#else
	if (doHex())
		printf("%I64uh\n", val);
	else
		printf("%I64u\n", val);
#endif
}

void PrintStream::println(float val) {
	printf("%f\n", val);
}

void PrintStream::println(void* val) {
	printf("%p\n", val);
}

void PrintStream::println(const char* str) {
	printf("%s\n", str);
}

void PrintStream::println(const char* str, int length) {
	printf("%.*s\n", length, str);
}

void PrintStream::println(const String& str) {
	printf("%s\n", str.toCharArray());
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

PrintStream& PrintStream::operator<< (const StreamFlags flags) {
	switch (flags) {
	case dec:
		streamFlags = (StreamFlags) ((uint32) streamFlags & ~(uint32) hex);
		break;
	case ::endl:
		println("");
		break;
	default:
		streamFlags = (StreamFlags) ((uint32) streamFlags | (uint32) flags);
		break;
	}

	return *this;
}
