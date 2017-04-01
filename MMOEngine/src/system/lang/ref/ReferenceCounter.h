/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REFERENCECOUNTER_H_
#define REFERENCECOUNTER_H_

#include <signal.h>

#include "system/platform.h"

#include "system/lang/System.h"

#include "system/thread/atomic/AtomicInteger.h"

namespace sys {
  namespace lang {

	class ReferenceCounter {
	protected:
		volatile uint32 _references;

	public:
		ReferenceCounter() {
		}

		ReferenceCounter(uint32 count) : _references(count) {
		}

		ReferenceCounter(const ReferenceCounter& counter) {
			_references = counter._references;
		}

	public:
		inline uint32 increaseCount() volatile {
			return AtomicInteger::add(&_references, 2);
		}

		void setLowestBit() volatile {
			uint32 oldVal, newVal;

			do {
				oldVal = _references;

				COMPILER_BARRIER();

				newVal = oldVal | 1;
			} while (!AtomicInteger::compareAndSet(&_references, oldVal, newVal));
		}

		void clearLowestBit() volatile {
			uint32 oldVal, newVal;

			do {
				oldVal = _references;

				COMPILER_BARRIER();

				newVal = oldVal - 1;
			} while (!AtomicInteger::compareAndSet(&_references, oldVal, newVal));
		}

		inline bool tryFinalDecrement() volatile {
			uint32 oldVal, newVal;

			oldVal = _references;

			COMPILER_BARRIER();

			if (oldVal != 2)
				return false;

			newVal = 1;

			return AtomicInteger::compareAndSet(&_references, oldVal, newVal);
		}

		inline uint32 decrementAndTestAndSet() volatile {
			uint32 oldVal, newVal;

			do {
				oldVal = _references;

				COMPILER_BARRIER();

				newVal = oldVal - 2;

				//assert(oldVal >= 2);

				if (newVal == 0)
					newVal = 1;
			} while (!AtomicInteger::compareAndSet(&_references, oldVal, newVal));

			return ((oldVal - newVal) & 1);
		}

		inline uint32 getReferenceCount() volatile {
			//WMB();

			return _references;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCECOUNTER_H_*/
