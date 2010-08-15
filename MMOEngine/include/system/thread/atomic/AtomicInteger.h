/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ATOMICINTEGER_H_
#define ATOMICINTEGER_H_

#include "system/platform.h"

#include "system/lang/Variable.h"

#include "system/io/ObjectInputStream.h"
#include "system/io/ObjectOutputStream.h"

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

	class AtomicInteger : public Variable {
		volatile uint32 value;

	public:
		AtomicInteger() {
			value = 0;
		}

		AtomicInteger(uint32 val) {
			value = val;
		}

		uint32 increment() {
			#if GCC_VERSION >= 40100
				return __sync_add_and_fetch(&value, 1);
			#elif defined(PLATFORM_MAC)
				return OSAtomicIncrement32((volatile int32_t*) &value);
			#elif PLATFORM_FREEBSD
				atomic_add_int(&value, 1);
				return value;
			#elif defined PLATFORM_LINUX
				//TODO: find appropriate method
				return ++(value);
			#elif defined PLATFORM_SOLARIS
				atomic_inc_uint(&value);
				return value;
			#elif defined PLATFORM_CYGWIN
				//TODO: find appropriate method
				return ++(value);
			#else
				return InterlockedIncrement((long*) &value);
			#endif
		}

		uint32 decrement() {
			#if GCC_VERSION >= 40100
				return __sync_sub_and_fetch(&value, 1);
			#elif defined(PLATFORM_MAC)
				return OSAtomicDecrement32((volatile int32_t*) &value);
			#elif PLATFORM_FREEBSD
				atomic_subtract_int(&value, 1);
				return value;
			#elif defined PLATFORM_LINUX
				//TODO: find appropriate method
				return --(value);
			#elif defined PLATFORM_SOLARIS
				atomic_dec_uint(&value);
				return value;
			#elif defined PLATFORM_CYGWIN
				//TODO: find appropriate method
				return --(value);
			#else
				return InterlockedDecrement((long*) &value);
			#endif
		}

		bool compareAndSet(uint32 oldval, uint32 newval) {
		#if GCC_VERSION >= 40100
			return __sync_bool_compare_and_swap(&value, oldval, newval);
		#elif defined(PLATFORM_MAC)
			return OSAtomicCompareAndSwapLong(oldvalue, newvalue, (volatile int32_t*) &value);
		#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
			//TODO: find appropriate method
			 if ( value == oldval ) {
				 value = newval;
			      return true;
			  } else {
			      return false;
			  }
		#else
			InterlockedCompareExchange(&value, newval, oldval);

			return value == newval;
		#endif
		}

		uint32 get() const {
			return value;
		}

		void set(uint32 val) {
			value = val;
		}

		uint32 operator=(const uint32 val) {
			return value = val;
		}

		operator uint32() const {
			return value;
		}

		bool toBinaryStream(sys::io::ObjectOutputStream* stream) {
			stream->writeInt(value);

			return true;
		}

		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream) {
			*this = stream->readInt();

			return true;
		}
	};

	} // namespace atomic
  } // namespace thread
} //namespace sys

using namespace sys::thread::atomic;

#endif /*ATOMICINTEGER_H_*/
