/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ATOMICREFERENCE_H_
#define ATOMICREFERENCE_H_

#include "AtomicInteger.h"
#include "AtomicLong.h"

namespace sys {
  namespace thread {
	namespace atomic {

	 template<class V> class AtomicReference {
	#ifdef PLATFORM_64
		AtomicLong reference;
	#else
		AtomicInteger reference;
	#endif

	public:
		AtomicReference() {
		}

		AtomicReference(V* ref) {
		#ifdef PLATFORM_64
			reference.set((uint64) ref);
		#else
			reference.set((uint32) ref);
		#endif
		}

		bool compareAndSet(V* oldref, V* newref) {
		#ifdef PLATFORM_64
			return reference.compareAndSet((uint64) oldref, (uint64) newref);
		#else
			return reference.compareAndSet((uint32) oldref, (uint32) newref);
		#endif
		}

		V* get() const {
		#ifdef PLATFORM_64
			return (V*) reference.get();
		#else
			return (V*) reference.get();
		#endif
		}

		void set(V* ref) {
		#ifdef PLATFORM_64
			reference.set((uint64) ref);
		#else
			reference.set((uint32) ref);
		#endif
		}

		operator V*() const {
			return get();
		}
	};

	} // namespace atomic
  } // namespace thread
} //namespace sys

using namespace sys::thread::atomic;

#endif /*ATOMICREFERENCE_H_*/
