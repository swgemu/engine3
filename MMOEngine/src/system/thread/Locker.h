/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef LOCKER_H_
#define LOCKER_H_

#include "Lockable.h"
#include "Mutex.h"
#include "ReadWriteLock.h"

namespace sys {
  namespace thread {

	  //use templates someday
	  class SCOPED_CAPABILITY Locker  {
		  Lockable* lockable;
	public:
		  Locker(Locker&& locker) : lockable(locker.lockable) {
		  	locker.lockable = nullptr;
		  }

		  Locker(const Locker&) = delete;
		  Locker& operator=(const Locker&) = delete;


		  Locker(Lockable* lock, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  if (doLock) {
				  lockable = lock;

				  lock->lock(true, file, function, line);
			  } else {
				  lockable = nullptr;
			  }
		  }

		  Locker(Mutex* lock, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  if (doLock) {
				  lockable = lock;

				  lock->lock(true, file, function, line);
			  } else {
				  lockable = nullptr;
			  }
		  }

		  Locker(ReadWriteLock* lock, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  if (doLock) {
				  lockable = lock;

				  lock->lock(true, file, function, line);
			  } else {
				  lockable = nullptr;
			  }
		  }

		  Locker(Mutex* lock, Mutex* cross, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  if (doLock) {
				  lockable = lock;

				  if (lock != cross) {
					  assert(cross->isLockedByCurrentThread());

					  lock->lock(cross, file, function, line);
				  } else {
					  lock->lock(true, file, function, line);
				  }
			  } else {
				  lockable = nullptr;
			  }
		  }

		 Locker(ReadWriteLock* lock, ReadWriteLock* cross, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  if (doLock) {
				  lockable = lock;

				  if (lock != cross) {
					  assert(cross->isLockedByCurrentThread());

					  lock->lock(cross, file, function, line);
				  } else {
					  lock->lock();
				  }
			  } else {
				  lockable = nullptr;
			  }
		  }

		 Locker(ReadWriteLock* lock, Mutex* cross, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  if (doLock) {
				  lockable = lock;

				  if (static_cast<Lockable*>(lock) != static_cast<Lockable*>(cross)) {
					  assert(cross->isLockedByCurrentThread());

					  lock->lock(cross, file, function, line);
				  } else {
					  lock->lock();
				  }
			  } else {
				  lockable = nullptr;
			  }
		  }

		 Locker(Mutex* lock, ReadWriteLock* cross, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  if (doLock) {
				  lockable = lock;

				  if (static_cast<Lockable*>(lock) != static_cast<Lockable*>(cross)) {
					  assert(cross->isLockedByCurrentThread());

					  lock->lock(cross, file, function, line);
				  } else {
					  lock->lock();
				  }
			  } else {
				  lockable = nullptr;
			  }
		  }

		  Locker(Lockable* lock, Lockable* cross, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE(lock) {
			  const auto doLock = !lock->isLockedByCurrentThread();

			  if (doLock) {
				  lockable = lock;

				  if (lock != cross) {
					  assert(cross->isLockedByCurrentThread());

					  lock->lock(cross, file, function, line);
				  } else {
					  lock->lock();
				  }
			  } else {
				  lockable = nullptr;
			  }
		  }

		  ~Locker() RELEASE() {
			  if (lockable != nullptr) {
				  lockable->unlock();
			  }
		  }

		  inline void release(const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) RELEASE() {
			  if (lockable != nullptr) {
				  lockable->unlock(true, file, function, line);

				  lockable = nullptr;
			  }
		  }
	  };

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*LOCKER_H_*/
