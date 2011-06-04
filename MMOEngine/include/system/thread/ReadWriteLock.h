/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef READWRITELOCK_H_
#define READWRITELOCK_H_

#include "Lockable.h"

namespace sys {
  namespace thread {

	class Mutex;

	class ReadWriteLock : public Lockable  {
	protected:
		pthread_rwlock_t rwlock;

	public:
		ReadWriteLock() : Lockable() {
			pthread_rwlock_init(&rwlock, NULL);
		}

		ReadWriteLock(const String& s) : Lockable(s) {
			pthread_rwlock_init(&rwlock, NULL);
		}

		ReadWriteLock(const ReadWriteLock& s) : Lockable() {
			pthread_rwlock_init(&rwlock, NULL);
		}

		virtual ~ReadWriteLock() {
			pthread_rwlock_destroy(&rwlock);
		}

		inline void lock(bool doLock = true) {
			wlock(doLock);
		}

		void rlock(bool doLock = true);

		void wlock(bool doLock = true);
		void wlock(Mutex* lock);
		void wlock(ReadWriteLock* lock);

		void lock(Lockable* lockable);

		inline bool tryWLock() {
			return pthread_rwlock_trywrlock(&rwlock) == 0;
		}

		void unlock(bool doLock = true);

		void runlock(bool doLock = true);

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
