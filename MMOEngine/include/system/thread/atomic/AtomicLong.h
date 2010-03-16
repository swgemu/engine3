/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ATOMICLONG_H_
#define ATOMICLONG_H_

#include "system/platform.h"

#ifdef PLATFORM_MAC
#include <libkern/OSAtomic.h>
#elif PLATFORM_FREEBSD
#include <machine/atomic.h>
#elif defined PLATFORM_SOLARIS
#include <atomic.h>
#endif

namespace sys {
  namespace thread {
	namespace atomic {

	class AtomicLong {
		volatile uint64 value;

	public:
		AtomicLong() {
			value = 0;
		}

		AtomicLong(uint64 val) {
			value = val;
		}

		uint64 increment() {
			#if GCC_VERSION >= 40100
				return __sync_add_and_fetch(&value, 1);
			#elif defined(PLATFORM_MAC)
				return OSAtomicIncrement64((volatile int64_t*) &value);
			#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
				return ++(value);
			#else
				return InterlockedIncrement64((long*) &value);
			#endif
		}

		uint64 decrement() {
			#if GCC_VERSION >= 40100
				return __sync_sub_and_fetch(&value, 1);
			#elif defined(PLATFORM_MAC)
				return OSAtomicDecrement32((volatile int32_t*) &value);
			#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
				//TODO: find appropriate method
				return --(value);
			#else
				return InterlockedDecrement((long*) &value);
			#endif
		}

		bool compareAndSet(uint64 oldval, uint64 newval) {
		#if GCC_VERSION >= 40100
			return __sync_bool_compare_and_swap (&value, oldval, newval);
		#elif defined(PLATFORM_MAC)
			return OSAtomicCompareAndSwap64(oldvalue, newvalue, (volatile int64_t*) &value);
		#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
			//TODO: find appropriate method
			 if ( value == oldval ) {
				 value = newval;
			      return true;
			  } else {
			      return false;
			  }
		#else
			InterlockedCompareExchange64(&value, newval, oldval);

			return value == newval;
		#endif
		}

		uint64 get() const {
			return value;
		}

		void set(uint64 val) {
			value = val;
		}

		operator uint64() const {
			return value;
		}
	};

	} // namespace atomic
  } // namespace thread
} //namespace sys

using namespace sys::thread::atomic;

#endif /*ATOMICLONG_H_*/
