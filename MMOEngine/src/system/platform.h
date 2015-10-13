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

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#ifdef _MSC_VER
#pragma warning (disable : 4018)
#pragma warning (disable : 4800)
#pragma warning (disable : 4244)
#pragma warning (disable : 4267)
#pragma warning (disable : 4996)
#pragma warning (disable : 4800)
#pragma warning (disable : 4548)
#pragma warning (disable : 4355)
#pragma warning (disable : 4180)

#undef snprintf
#define snprintf _snprintf

#define _USE_MATH_DEFINES

#define round(x) floor(x + 0.5)

#ifdef _WIN64
#undef PLATFORM_32
#define PLATFORM_64
#endif

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
#include <winsock2.h>
#include <windows.h>
#endif

#include <time.h>

#include <cstddef>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <memory>
#include <limits>
#include <float.h>
#include <assert.h>

#ifdef PLATFORM_SOLARIS
#include <Strings.h>
#endif

#ifdef PLATFORM_MAC
#include <math.h>

extern "C" int isnan (double);
extern "C" int isinf (double);
#endif

#ifdef PLATFORM_WIN
#include <float.h>
#define isnan(X) _isnan(X)
#define isinf(x) (!_finite(x) && !_isnan(x))
#endif

//#define WMB() __asm__ __volatile__ ("" ::: "memory");
#if GCC_VERSION >= 40100
#define WMB() __sync_synchronize()
#else
#define WMB()
#endif

#ifdef __clang__
#define CLANG_COMPILER
#endif

#if __cplusplus >= 201103L
#define	CXX11_COMPILER
#include <utility>
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

#ifndef __MINGW32__
	typedef uint8 byte;
#endif

#ifdef PLATFORM64
	typedef uint64 pointer;
#else
	typedef uint32 pointer;
#endif

	#define MAX(a, b) (a > b ? a : b)
	#define MIN(a, b) (a < b ? a : b)

	#define SQR(x) ((x) * (x))

	#define MAX_FLOAT FLT_MAX
	#define SMALL_NUM  FLT_EPSILON
	
	#ifdef DEFAULT_DYNAMIC_CAST
	#define cast dynamic_cast
	#else
	#define cast static_cast
	#endif

	//#define LOG_LOCKS
	//#define TRACE_LOCKS

	//#define TRACE_REFERENCING
	#define LINE_TRACING
	//#define TRACE_REFERENCES

	//#define TRACE_TASKS

	//#define WITH_STM
	//#define MEMORY_PROTECTION

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

	namespace security {
	}

} // namespace sys

using namespace sys;

#endif /*PLATFORM_H_*/
