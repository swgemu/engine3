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
		AtomicInteger _references;

	public:
		ReferenceCounter() {
		}

		ReferenceCounter(uint32 count) : _references(count) {
		}

		ReferenceCounter(const ReferenceCounter& counter) {
			_references = counter._references;
		}

		virtual ~ReferenceCounter() {
		}

	public:
		inline uint32 increaseCount() {
			return _references.add(2);
		}

		void clearLowestBit() {
			uint32 oldVal, newVal;

			do {
				oldVal = _references;
				newVal = oldVal - 1;
			} while (!_references.compareAndSet(oldVal, newVal));
		}

		inline uint32 decrementAndTestAndSet() {
			uint32 oldVal, newVal;

			do {
				oldVal = _references;
				newVal = oldVal - 2;

				//assert(oldVal >= 2);

				if (newVal == 0)
					newVal = 1;
			} while (!_references.compareAndSet(oldVal, newVal));

			return ((oldVal - newVal) & 1);
		}

		inline uint32 getReferenceCount() {
			WMB();

			return _references.get();
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCECOUNTER_H_*/
