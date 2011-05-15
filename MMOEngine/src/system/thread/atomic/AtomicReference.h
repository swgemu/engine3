/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ATOMICREFERENCE_H_
#define ATOMICREFERENCE_H_

#include "AtomicInteger.h"
#include "AtomicLong.h"

#include "system/lang/ref/Reference.h"

namespace sys {
  namespace thread {
	namespace atomic {

	 template<class V> class AtomicReference {
	#ifdef PLATFORM_64
		AtomicLong reference;
	#else
		AtomicInteger reference;
	#endif

		Reference<V*> objectReference;

	public:
		AtomicReference() {
		}

		AtomicReference(V* ref) {
			reference.set(toReferenceValue(ref));
		}

		bool compareAndSet(V* oldref, V* newref) {
			if (reference.compareAndSet(toReferenceValue(oldref), toReferenceValue(newref))) {
				//objectReference = newref;

				return true;
			} else
				return false;
		}

		V* get() const {
			return (V*) reference.get();
		}

		void set(V* ref) {
			reference.set(toReferenceValue(ref));

			//objectReference = ref;
		}

		V* operator=(V* ref) {
			set(ref);

			return ref;
		}

		operator V*() const {
			return get();
		}

		V* operator->() const {
			return get();
		}

	protected:
	#ifdef PLATFORM_64
		uint64 toReferenceValue(V* ref) {
			return (uint64) ref;
		}
	#else
		uint32 toReferenceValue(V* ref) {
			return (uint32) ref;
		}
	#endif

	};

	} // namespace atomic
  } // namespace thread
} //namespace sys

using namespace sys::thread::atomic;

#endif /*ATOMICREFERENCE_H_*/
