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

		Locker(Lockable* lockable, Lockable* cross) {
			acquire(lockable, cross);
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

		inline void acquire(Lockable* lockable, Lockable* cross) {
			Locker::doLock = !lockable->isLockedByCurrentThread();

			Locker::lockable = lockable;

			if (doLock && lockable != cross) {
				lockable->lock(cross);
			} else {
				doLock = false;
			}
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
