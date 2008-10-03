/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ATOMIC_H_
#define ATOMIC_H_

#include "../platform.h"

#ifdef PLATFORM_FREEBSD
#include <machine/atomic.h>
#elif defined PLATFORM_SOLARIS
#include <atomic.h>
#endif

namespace sys {
  namespace thread {

	class Atomic {
	public:
		static inline void incrementInt(uint32* value) {
			#if GCC_VERSION >= 40100
				__sync_add_and_fetch(value, 1);
			#elif PLATFORM_FREEBSD
				atomic_add_int(value, 1);
			#elif defined PLATFORM_LINUX
				//TODO: find appropriate method
				++(*value);
			#elif defined PLATFORM_SOLARIS
				atomic_inc_uint(value);
			#elif defined PLATFORM_CYGWIN
				//TODO: find appropriate method
				++(*value);
			#else
				InterlockedIncrement((long*)value);
			#endif
		}

		static inline bool decrementInt(uint32* value) {
			#if GCC_VERSION >= 40100
				return __sync_sub_and_fetch(value, 1);
			#elif PLATFORM_FREEBSD
				atomic_subtract_int(value, 1);
				return *value;
			#elif defined PLATFORM_LINUX
				//TODO: find appropriate method
				return --(*value);
			#elif defined PLATFORM_SOLARIS
				atomic_dec_uint(value);
				return *value;
			#elif defined PLATFORM_CYGWIN
				//TODO: find appropriate method
				return --(*value);
			#else
				return InterlockedDecrement((long*) value);
			#endif
		}

	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*ATOMIC_H_*/
