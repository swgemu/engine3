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

		ReferenceCounter(ReferenceCounter& counter) {
			_references = counter._references;
		}

		virtual ~ReferenceCounter() {
			if (getReferenceCount() > 1) {
				System::out << "WARNING - reference count was not zero on delete\n";
				StackTrace::printStackTrace();
			}
		}

	protected:
		inline void increaseCount() {
			_references.increment();
		}

		inline bool decreaseCount() {
			if (getReferenceCount() < 1) {
				System::out << "WARNING - reference count getting under zero\n";
				raise(SIGSEGV);
			}

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
