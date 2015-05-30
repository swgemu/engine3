/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ATOMICREFERENCE_H_
#define ATOMICREFERENCE_H_

#include "system/platform.h"

//#include "system/lang/ref/Reference.h"

namespace sys {
  namespace thread {
	namespace atomic {

	 template<class V> class AtomicReference {
		 volatile V value;

	public:
		AtomicReference() {
			value = NULL;
		}

		AtomicReference(V ref) {
			value = ref;
		}

		AtomicReference(const AtomicReference& r) : value(r.value) {

		}

		inline V compareAndSetReturnOld(volatile void* oldval, void* newval) {
#ifdef CLANG_COMPILER
#ifdef PLATFORM_64
			volatile long long* addy = (volatile long long*) &value;

			return (V) __sync_val_compare_and_swap (addy, (uint64)oldval, (uint64)newval);
#else
			volatile int* addy = (volatile int*) &value;

			return (V) __sync_val_compare_and_swap (addy, (uint32)oldval, (uint32)newval);
#endif
#elif GCC_VERSION >= 40100
			return __sync_val_compare_and_swap((void**)&value, (void*)oldval, (void*)newval);
#else
			PVOID* oldVal = (PVOID*)value;
			InterlockedCompareExchangePointer((volatile PVOID*)&oldVal, newval, (PVOID*) oldval);

			return (V) oldVal;
#endif
		}

		inline bool compareAndSet(volatile void* oldval, void* newval) {
#ifdef CLANG_COMPILER
#ifdef PLATFORM_64
//			void* blia = value;

			volatile long long* addy = (volatile long long*) &value;

			return __sync_bool_compare_and_swap (addy, (uint64)oldval, (uint64)newval);
#else
			volatile int* addy = (volatile int*) &value;

			return __sync_bool_compare_and_swap (addy, (uint32)oldval, (uint32)newval);
#endif
#elif GCC_VERSION >= 40100 || defined(__MINGW32__)
			return __sync_bool_compare_and_swap ((void**)&value, (void*)oldval, (void*)newval);
#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
			//TODO: find appropriate method
			if ( value == oldval ) {
				value = newval;
				return true;
			} else {
				return false;
			}
#else
			InterlockedCompareExchangePointer((volatile PVOID *)&value, newval, (PVOID *)oldval);

			return value == newval;
#endif
		}

		template<typename T>
		static inline T compareAndSetReturnOld(volatile T* value, volatile void* oldval, void* newval) {
		#ifdef CLANG_COMPILER
		#ifdef PLATFORM_64
					volatile long long* addy = (volatile long long*) value;

					return (V) __sync_val_compare_and_swap (addy, (uint64)oldval, (uint64)newval);
		#else
					volatile int* addy = (volatile int*) value;

					return (V) __sync_val_compare_and_swap (addy, (uint32)oldval, (uint32)newval);
		#endif
		#elif GCC_VERSION >= 40100
					return __sync_val_compare_and_swap((void**)value, (void*)oldval, (void*)newval);
		#else
					PVOID* oldVal = (PVOID*)value;
					InterlockedCompareExchangePointer((volatile PVOID*)&oldVal, newval, (PVOID*) oldval);

					return (V) oldVal;
		#endif
				}

		template<typename T>
		static inline bool compareAndSet(volatile T* value, volatile void* oldval, void* newval) {
		#ifdef CLANG_COMPILER
		#ifdef PLATFORM_64
		//			void* blia = value;

					volatile long long* addy = (volatile long long*) value;

					return __sync_bool_compare_and_swap (addy, (uint64)oldval, (uint64)newval);
		#else
					volatile int* addy = (volatile int*) value;

					return __sync_bool_compare_and_swap (addy, (uint32)oldval, (uint32)newval);
		#endif
		#elif GCC_VERSION >= 40100 || defined(__MINGW32__)
					return __sync_bool_compare_and_swap ((void**)value, (void*)oldval, (void*)newval);
		#elif defined(PLATFORM_FREEBSD) || defined(PLATFORM_LINUX) || defined(PLATFORM_SOLARIS) || defined(PLATFORM_CYGWIN)
					//TODO: find appropriate method
					if ( *value == *oldval ) {
						*value = *newval;
						return true;
					} else {
						return false;
					}
		#else
					InterlockedCompareExchangePointer((volatile PVOID *)&value, newval, (PVOID *)oldval);

					return value == newval;
		#endif
		}

		inline V get() const {
			//WMB();

			return value;
		}

		void set(V val) {
			value = val;
		}

		V operator=(V ref) {
			set(ref);

			return ref;
		}

		operator V() const {
			return get();
		}

		inline V operator->() const {
			return get();
		}

		inline bool operator== (const V val) const {
			WMB();

			return val == value;
		}

	};

	} // namespace atomic
  } // namespace thread
} //namespace sys

using namespace sys::thread::atomic;

#endif /*ATOMICREFERENCE_H_*/
