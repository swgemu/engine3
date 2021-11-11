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

		ReadWriteLock& operator=(const ReadWriteLock& lock) {
			return *this;
		}

		~ReadWriteLock() {
			pthread_rwlock_destroy(&rwlock);
		}

		inline void lock(bool doLock = true, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE() {
			wlock(doLock, file, function, line);
		}

		virtual void rlock(bool doLock = true, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE_SHARED();

		virtual void wlock(bool doLock = true, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE();
		virtual void wlock(Mutex* lock, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE();
		virtual void wlock(ReadWriteLock* lock, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE();
		virtual void rlock(ReadWriteLock* lock, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE_SHARED();
		virtual void rlock(Lockable* lock, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE())  ACQUIRE_SHARED();

		void lock(Lockable* lockable, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE();

		inline void lock(ReadWriteLock* lockable, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE() {
			wlock(lockable, file, function, line);
		}

		inline void lock(Mutex* lockable, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE() {
			wlock(lockable, file, function, line);
		}

		inline bool tryWLock(const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) TRY_ACQUIRE(true) {
			lockAcquiring("w", file, function, line);

			if (pthread_rwlock_trywrlock(&rwlock) == 0) {
				lockAcquired("w", file, function, line);

				return true;
			} else {
				return false;
			}
		}

		void unlock(bool doLock = true, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) RELEASE();

		virtual void runlock(bool doLock = true, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) RELEASE_SHARED();

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
