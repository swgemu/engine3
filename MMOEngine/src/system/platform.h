/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#ifdef LOCKFREE_BCLIENT_BUFFERS
#include <boost/lockfree/queue.hpp>
#endif

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

#include <ctime>
#include <cstddef>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <memory>
#include <limits>
#include <cfloat>
#include <cassert>

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

#if GCC_VERSION >= 40100
#define WMB() __sync_synchronize()
#define COMPILER_BARRIER() __asm__ __volatile__ ("" ::: "memory");
#else
#define WMB()
#define COMPILER_BARRIER()
#endif

#ifdef __clang__
#define E3_CLANG_COMPILER
#endif

#if __cplusplus >= 201103L
#define	CXX11_COMPILER
#include <utility>
#endif

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   // no-op
#endif

//#define THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))

#define CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY \
  THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define ACQUIRED_BEFORE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define REQUIRES_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RELEASE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RELEASE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define EXCLUDES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RETURN_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS \
  THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

#ifdef USE_LOCK_STYLE_THREAD_SAFETY_ATTRIBUTES
// The original version of thread safety analysis the following attribute
// definitions.  These use a lock-based terminology.  They are still in use
// by existing thread safety code, and will continue to be supported.

// Deprecated.
#define PT_GUARDED_VAR \
  THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_var)

// Deprecated.
#define GUARDED_VAR \
  THREAD_ANNOTATION_ATTRIBUTE__(guarded_var)

// Replaced by REQUIRES
#define EXCLUSIVE_LOCKS_REQUIRED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(exclusive_locks_required(__VA_ARGS__))

// Replaced by REQUIRES_SHARED
#define SHARED_LOCKS_REQUIRED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(shared_locks_required(__VA_ARGS__))

// Replaced by CAPABILITY
#define LOCKABLE \
  THREAD_ANNOTATION_ATTRIBUTE__(lockable)

// Replaced by SCOPED_CAPABILITY
#define SCOPED_LOCKABLE \
  THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

// Replaced by ACQUIRE
#define EXCLUSIVE_LOCK_FUNCTION(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(exclusive_lock_function(__VA_ARGS__))

// Replaced by ACQUIRE_SHARED
#define SHARED_LOCK_FUNCTION(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(shared_lock_function(__VA_ARGS__))

// Replaced by RELEASE and RELEASE_SHARED
#define UNLOCK_FUNCTION(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(unlock_function(__VA_ARGS__))

// Replaced by TRY_ACQUIRE
#define EXCLUSIVE_TRYLOCK_FUNCTION(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(exclusive_trylock_function(__VA_ARGS__))

// Replaced by TRY_ACQUIRE_SHARED
#define SHARED_TRYLOCK_FUNCTION(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(shared_trylock_function(__VA_ARGS__))

// Replaced by ASSERT_CAPABILITY
#define ASSERT_EXCLUSIVE_LOCK(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_exclusive_lock(__VA_ARGS__))

// Replaced by ASSERT_SHARED_CAPABILITY
#define ASSERT_SHARED_LOCK(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_lock(__VA_ARGS__))

// Replaced by EXCLUDE_CAPABILITY.
#define LOCKS_EXCLUDED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

// Replaced by RETURN_CAPABILITY
#define LOCK_RETURNED(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#endif  // USE_LOCK_STYLE_THREAD_SAFETY_ATTRIBUTES

#ifdef __BIG_ENDIAN__
static_assert(false, "big endian systems are not supported");
#endif

namespace sys {
	[[ noreturn ]] inline void e3_assert_internal(const char* file, int line, const char* func, const char *expression) {
		::fprintf(stderr, "assertion \"%s\" failed: file \"%s\", line %d%s%s\n", expression, file, line, func ? ", function: " : "", func ? func : "");

		::fflush(nullptr);

		::abort();
	}

	#define E3_ASSERT(_expr) (static_cast <bool> (_expr) ? void (0) : e3_assert_internal(__FILE__, __LINE__, __PRETTY_FUNCTION__, #_expr));
	#define E3_ABORT(_message) (e3_assert_internal(__FILE__, __LINE__, __PRETTY_FUNCTION__, _message));

	typedef unsigned long long uint64;
	static_assert(sizeof(uint64) == 8, "unsigned long long is not 64bit");

	typedef unsigned int uint32;
	static_assert(sizeof(uint32) == 4, "usigned int is not 32 bit");

	typedef unsigned short uint16;
	static_assert(sizeof(uint16) == 2, "unsigned short is not 16 bit");

	typedef unsigned char uint8;
	static_assert(sizeof(uint8) == 1, "unsigned char is not 8 bit");

	typedef signed long long int64;
	static_assert(sizeof(int64) == 8, "signed long long is not 64bit");

	typedef signed int int32;
	static_assert(sizeof(int32) == 4, "signed int is not 32 bit");

	typedef signed short int16;
	static_assert(sizeof(int16) == 2, "signed short is not 16 bit");

	typedef signed char int8;
	static_assert(sizeof(int8) == 1, "signed char is not 8 bit");

	static_assert(sizeof(float) == 4, "float is not 4 bytes long");

#ifndef __MINGW32__
	typedef uint8 byte;
#endif

#ifdef PLATFORM64
	typedef uint64 pointer;
#else
	typedef uint32 pointer;
#endif

	#define MAX_FLOAT FLT_MAX
	#define SMALL_NUM  FLT_EPSILON

	//#define VECTORS_OUT_OF_BOUNDS_CHECK

	//#ifdef DEFAULT_DYNAMIC_CAST
	#define cast dynamic_cast
	//#else
	//#define cast static_cast
	//#endif

	//#define LOG_LOCKS
	//#define TRACE_LOCKS

	//#define TRACE_REFERENCING
	#define LINE_TRACING
	//#define TRACE_REFERENCES

	//#define TRACE_TASKS

	//#define WITH_STM
	//#define MEMORY_PROTECTION

	#define BASECLIENT_DISABLE_STATSD

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

