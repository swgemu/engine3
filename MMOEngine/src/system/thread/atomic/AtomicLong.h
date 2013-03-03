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

		inline uint64 increment() {
			#if GCC_VERSION >= 40100 && defined(PLATFORM_64) || defined(__MINGW32__)
				return __sync_add_and_fetch(&value, 1);
			#elif defined(PLATFORM_MAC)
				return OSAtomicIncrement64((volatile int64_t*) &value);
			#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
				return ++(value);
			#else
				return InterlockedIncrement64((volatile LONGLONG *) &value);
			#endif
		}

		inline uint64 decrement() {
			#if GCC_VERSION >= 40100 && defined(PLATFORM_64) || defined(__MINGW32__)
				return __sync_sub_and_fetch(&value, 1);
			#elif defined(PLATFORM_MAC)
				return OSAtomicDecrement64((volatile int64_t*) &value);
			#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
				//TODO: find appropriate method
				return --(value);
			#else
				return InterlockedDecrement((long*) &value);
			#endif
		}

		inline uint64 add(uint64 val) {
			#if GCC_VERSION >= 40100 && defined(PLATFORM_64) || defined(__MINGW32__)
				return __sync_add_and_fetch(&value, val);
			#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
				return value += val;
			#else
				return InterlockedAdd64((volatile LONGLONG *) &value, val);
			#endif
		}

		inline bool compareAndSet(uint64 oldval, uint64 newval) {
		#if GCC_VERSION >= 40100 && defined(PLATFORM_64) || defined(__MINGW32__)
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
			InterlockedCompareExchange64((volatile LONGLONG *)&value, newval, oldval);

			return value == newval;
		#endif
		}

		inline uint64 get() const {
			WMB();

			return value;
		}

		inline void set(uint64 val) {
			while (!compareAndSet(value, val)) ;
		}

		operator uint64() const {
			WMB();

			return value;
		}

		inline bool operator== (const uint64 val) const {
			WMB();

			return val == value;
		}
		
		bool toBinaryStream(sys::io::ObjectOutputStream* stream) {
		        stream->writeLong(value);
		        
		        return true;
		}
		
		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream) {
		        *this = stream->readLong();
		        
		        return true;
                }
	};

	} // namespace atomic
  } // namespace thread
} //namespace sys

using namespace sys::thread::atomic;

#endif /*ATOMICLONG_H_*/
