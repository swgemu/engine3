/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REFERENCECOUNTER_H_
#define REFERENCECOUNTER_H_

#include <signal.h>

#include "../../platform.h"

#include "../System.h"

#include "../../thread/Atomic.h"

namespace sys {
  namespace lang {

	class ReferenceCounter {
	protected:
		uint32* _references;

	public:
		ReferenceCounter() {
			_references = NULL;
		}

		ReferenceCounter(ReferenceCounter& counter) {
			_references = counter._references;
		}

		virtual ~ReferenceCounter() {
			finalizeCount();
		}

	protected:
		inline void initializeCount() {
			_references = new uint32();
			*_references = 1;
		}

		inline void finalizeCount() {
			if (_references != NULL) {
				if (getReferenceCount() > 1) {
					System::out << "WARNING - reference count was not zero on delete\n";
					StackTrace::printStackTrace();
				}

				delete _references;
				_references = NULL;
			}
		}

		inline void increaseCount() {
			Atomic::incrementInt(_references);
		}

		inline bool decreaseCount() {
			if (getReferenceCount() < 1) {
				System::out << "WARNING - reference count getting under zero\n";
				raise(SIGSEGV);
			}

			return !Atomic::decrementInt(_references);
		}

	public:
		inline uint32 getReferenceCount() {
			return *_references;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCECOUNTER_H_*/
