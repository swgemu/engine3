/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef READWRITELOCK_H_
#define READWRITELOCK_H_

#include "Lockable.h"

namespace sys {
  namespace thread {

	class Mutex;

	class CAPABILITY("mutex") ReadWriteLock : public Lockable  {
	protected:
		pthread_rwlock_t rwlock;

	public:
		ReadWriteLock() : Lockable() {
			pthread_rwlock_init(&rwlock, nullptr);
		}

		ReadWriteLock(const String& s) : Lockable(s) {
			pthread_rwlock_init(&rwlock, nullptr);
		}

		ReadWriteLock(const ReadWriteLock& s) : Lockable() {
			pthread_rwlock_init(&rwlock, nullptr);
		}

		~ReadWriteLock() {
			pthread_rwlock_destroy(&rwlock);
		}

		inline void lock(bool doLock = true) ACQUIRE() {
			wlock(doLock);
		}

		virtual void rlock(bool doLock = true) ACQUIRE_SHARED();

		virtual void wlock(bool doLock = true) ACQUIRE();
		virtual void wlock(Mutex* lock) ACQUIRE();
		virtual void wlock(ReadWriteLock* lock) ACQUIRE();
		virtual void rlock(ReadWriteLock* lock) ACQUIRE_SHARED();
		virtual void rlock(Lockable* lock)  ACQUIRE_SHARED();

		void lock(Lockable* lockable) ACQUIRE();

		inline void lock(ReadWriteLock* lockable) ACQUIRE() {
			wlock(lockable);
		}

		inline void lock(Mutex* lockable) ACQUIRE() {
			wlock(lockable);
		}

		inline bool tryWLock() TRY_ACQUIRE(true) {
			if (pthread_rwlock_trywrlock(&rwlock) == 0) {
				lockAcquired("w");

				return true;
			} else {
				return false;
			}
		}

		void unlock(bool doLock = true) RELEASE();

		virtual void runlock(bool doLock = true) RELEASE_SHARED();

		inline bool destroy() {
			pthread_rwlock_wrlock(&rwlock);
			pthread_rwlock_unlock(&rwlock);

			return pthread_rwlock_destroy(&rwlock) == 0;
		}

		friend class Condition;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*READWRITELOCK_H_*/
