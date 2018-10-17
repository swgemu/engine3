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

	public:
		  Locker(Locker&& locker) : lockable(locker.lockable) {
		  	locker.lockable = nullptr;
		  }

		  Locker(const Locker&) = delete;
		  Locker& operator=(const Locker&) = delete;


		  Locker(Lockable* lock) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  if (doLock) {
				  Locker::lockable = lock;

				  lock->lock(doLock);
			  } else {
				  Locker::lockable = nullptr;
			  }
		  }

		  Locker(Lockable* lock, Lockable* cross) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  Locker::lockable = lock;

			  if (doLock && lock != cross) {
				  Locker::lockable = lock;

				  assert(cross->isLockedByCurrentThread());

				  lock->lock(cross);
			  } else {
				  lockable = nullptr;
			  }
		  }

		  ~Locker() RELEASE() {
			  if (lockable != nullptr) {
				  lockable->unlock();
			  }
		  }

		  inline void release() RELEASE() {
			  if (lockable != nullptr) {
				  lockable->unlock();

				  lockable = nullptr;
			  }
		  }
	  };

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*LOCKER_H_*/
