/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ATOMICBOOLEAN_H_
#define ATOMICBOOLEAN_H_

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

	class AtomicBoolean : public Variable {
		volatile uint32 value;

	public:
		AtomicBoolean() {
			value = 0;
		}

		AtomicBoolean(bool val) {
			value = (uint32) val;
		}

		static bool compareAndSet(volatile bool* address, bool oldval, bool newval) {
		#if GCC_VERSION >= 40100 && !defined(PLATFORM_WIN)
			return __sync_bool_compare_and_swap(address, oldval, newval);
		#elif defined(PLATFORM_MAC)
			return OSAtomicCompareAndSwapLong((uint32) oldvalue, (uint32) newvalue, (volatile int32_t*) address);
		#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
			//TODO: find appropriate method
			 if ( *address == oldval ) {
				 *address = newval;
			      return true;
			  } else {
			      return false;
			  }
		#else
			InterlockedCompareExchange((volatile LONG*)address, (uint32) newval, (uint32) oldval);

			return *address == newval;
		#endif
		}

		bool compareAndSet(bool oldval, bool newval) {
		#if GCC_VERSION >= 40100 && !defined(PLATFORM_WIN)
			return __sync_bool_compare_and_swap(&value, (uint32) oldval, (uint32) newval);
		#elif defined(PLATFORM_MAC)
			return OSAtomicCompareAndSwapLong((uint32) oldvalue, (uint32) newvalue, (volatile int32_t*) &value);
		#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
			//TODO: find appropriate method
			 if ( value == (uint32) oldval ) {
				 value = (uint32) newval;
			      return true;
			  } else {
			      return false;
			  }
		#else
			InterlockedCompareExchange((volatile LONG*)&value, (uint32) newval, (uint32) oldval);

			return value == (uint32) newval;
		#endif
		}

		inline bool get() const {
			COMPILER_BARRIER();

			return (bool) value;
		}

		void set(bool val) {
			value = (uint32) val;
		}

		bool operator=(const bool val) {
			return (bool) (value = (uint32) val);
		}

		inline bool operator== (const bool val) const {
			COMPILER_BARRIER();

			return val == (bool)value;
		}

		operator bool() const {
			COMPILER_BARRIER();

			return (bool) value;
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

#endif /* ATOMICBOOLEAN_H_ */
