/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(__WIN32__)
	#define PLATFORM_WIN
#elif defined __CYGWIN__
	#define PLATFORM_CYGWIN
#elif defined __FreeBSD__
	#define PLATFORM_FREEBSD
	#define PLATFORM_UNIX
#elif defined (__SVR4) && defined (__sun)
	#define PLATFORM_SOLARIS
	#define PLATFORM_UNIX
#elif defined(__APPLE__)
	#define PLATFORM_MAC
	#define PLATFORM_UNIX
#else
	#define PLATFORM_LINUX
	#define PLATFORM_UNIX
#endif

#ifdef __x86_64__
#define PLATFORM_64
#else
#define PLATFORM_32
#endif

#define GCC_VERSION (__GNUC__ * 10000 \
								+ __GNUC_MINOR__ * 100 \
								+ __GNUC_PATCHLEVEL__)

#ifdef PLATFORM_WIN
#pragma warning (disable : 4018)
#pragma warning (disable : 4800)
#pragma warning (disable : 4244)
#pragma warning (disable : 4267)
#pragma warning (disable : 4996)
#pragma warning (disable : 4800)
#pragma warning (disable : 4548)

#define WIN32_LEAN_AND_MEAN
#endif

#ifdef PLATFORM_SOLARIS
#define BSD_COMP
#endif

#ifndef PLATFORM_WIN
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#else
#include <windows.h>
#endif

#include <time.h>

#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <memory>

#ifdef PLATFORM_SOLARIS
#include <Strings.h>
#endif

namespace sys {
	typedef unsigned long long uint64;
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;

	typedef signed long long int64;
	typedef signed int int32;
	typedef signed short int16;
	typedef signed char int8;

	typedef uint8 byte;

#ifdef PLATFORM64
	typedef uint64 pointer;
#else
	typedef uint32 pointer;
#endif

	#define MAX(a, b) (a > b ? a : b)
	#define MIN(a, b) (a < b ? a : b)

	//#define LOG_LOCKS
	//#define TRACE_LOCKS

	//#define TRACE_REFERENCING
	#define LINE_TRACING
	//#define TRACE_REFERENCES

	#ifdef VERSION_PUBLIC
	#define CONNECTION_LIMIT 20
	#endif

	namespace lang {
	} // namespace net

	namespace thread {
	} // namespace net

	namespace net {
	} // namespace net

	namespace util {
	} // namespace net

	namespace io {
	} // namespace net

} // namespace sys

using namespace sys;

#endif /*PLATFORM_H_*/
