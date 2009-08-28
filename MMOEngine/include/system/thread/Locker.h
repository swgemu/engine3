/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOCKER_H_
#define LOCKER_H_

#include "Lockable.h"

namespace sys {
  namespace thread {

	class Locker  {
		Lockable* lockable;

		bool doLock;

	public:
		Locker(Lockable* lockable) {
			acquire(lockable);
		}

		virtual ~Locker() {
			release();
		}

	protected:
		inline void acquire(Lockable* lockable) {
			Locker::doLock = !lockable->isLockedByCurrentThread();

			Locker::lockable = lockable;
			lockable->lock(doLock);
		}

	public:
		inline void release() {
			if (lockable != NULL) {
				lockable->unlock(doLock);
				lockable = NULL;
			}
		}
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*LOCKER_H_*/
