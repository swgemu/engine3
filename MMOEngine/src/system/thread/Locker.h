/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOCKER_H_
#define LOCKER_H_

#include "Lockable.h"

namespace sys {
  namespace thread {

	class SCOPED_CAPABILITY Locker  {
		Lockable* lockable;

		bool doLock;

	public:
		Locker(Lockable* lock) ACQUIRE(lock) {
			Locker::doLock = !lock->isLockedByCurrentThread();

			Locker::lockable = lock;
			lock->lock(doLock);
		}

		Locker(Lockable* lock, Lockable* cross) ACQUIRE(lock) {
			Locker::doLock = !lock->isLockedByCurrentThread();

			Locker::lockable = lock;

			if (doLock && lock != cross) {
				assert(cross->isLockedByCurrentThread());

				lock->lock(cross);
			} else {
				doLock = false;
			}
		}

		virtual ~Locker() RELEASE() {
			if (lockable != NULL) {
				lockable->unlock(doLock);
				lockable = NULL;
			}
		}

	public:
		inline void release() RELEASE() {
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
