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
#ifdef MEMORY_PROTECTION
		AtomicInteger* _references;
#else
		AtomicInteger _references;
#endif

	public:
		ReferenceCounter() {
#ifdef MEMORY_PROTECTION
			_references = new AtomicInteger();
#endif
		}

		ReferenceCounter(const ReferenceCounter& counter) {
#ifdef MEMORY_PROTECTION
			_references = new AtomicInteger();
#endif
		}

		virtual ~ReferenceCounter() {
			if (getReferenceCount() > 1) {
				StackTrace::printStackTrace();
				assert(0 && "reference count was not zero on delete");
			}

#ifdef MEMORY_PROTECTION
			delete _references;
			//_references = (AtomicInteger*)(uint64)-1;
			_references = NULL;
#endif
		}

	protected:
		inline void increaseCount() {
#ifdef MEMORY_PROTECTION
			_references->increment();
#else
			_references.increment();
#endif
		}

		inline bool decreaseCount() {
			/*if (getReferenceCount() < 1)
				assert(0 && "reference count getting under zero");*/
#ifdef MEMORY_PROTECTION
			return _references->decrement() == 0;
#else
			return _references.decrement() == 0;
#endif
		}

	public:
		inline uint32 getReferenceCount() {
#ifdef MEMORY_PROTECTION
			return _references->get();
#else
			return _references.get();
#endif
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCECOUNTER_H_*/
