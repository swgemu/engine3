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

		ReferenceCounter(const ReferenceCounter& counter) {
			//_references = counter._references; //this is wrong!
		}

		virtual ~ReferenceCounter() {
			if (getReferenceCount() > 1) {
				StackTrace::printStackTrace();
				assert(0 && "reference count was not zero on delete");
			}
		}

	protected:
		inline void increaseCount() {
			_references.increment();
		}

		inline bool decreaseCount() {
			/*if (getReferenceCount() < 1)
				assert(0 && "reference count getting under zero");*/

			return _references.decrement() == 0;
		}

	public:
		inline uint32 getReferenceCount() {
			return _references;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCECOUNTER_H_*/
