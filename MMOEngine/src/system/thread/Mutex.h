/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "Lockable.h"

namespace sys {
  namespace thread {

	class ReadWriteLock;

	class CAPABILITY("mutex") Mutex : public Lockable {
		pthread_mutex_t mutex;
		pthread_mutexattr_t attr;

	public:
		Mutex() : Lockable() {
			pthread_mutexattr_init(&attr);
			pthread_mutex_init(&mutex, &attr);
		}

		explicit Mutex(bool recursive) : Lockable() {
			pthread_mutexattr_init(&attr);

			if (recursive) {
				pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			}

			pthread_mutex_init(&mutex, &attr);

		}

		explicit Mutex(const char* s) : Lockable(s) {
			pthread_mutexattr_init(&attr);
			pthread_mutex_init(&mutex, &attr);
		}

		explicit Mutex(const String& s) : Lockable(s) {
			pthread_mutexattr_init(&attr);
			pthread_mutex_init(&mutex, &attr);
		}

		Mutex(const Mutex& m) : Lockable() {
			pthread_mutexattr_init(&attr);
			pthread_mutex_init(&mutex, &attr);
		}

		Mutex& operator=(const Mutex& m) {
			return *this;
		}

		~Mutex() {
			pthread_mutex_destroy(&mutex);
			pthread_mutexattr_destroy(&attr);
		}

		void lock(bool doLock = true, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE();
		void lock(Mutex* m, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE();
		void lock(Lockable* lockable, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) ACQUIRE();

		bool tryLock(const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) TRY_ACQUIRE(true);

		bool tryLock(uint64 time, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) TRY_ACQUIRE(true);

		void unlock(bool doLock = true, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) RELEASE();

		friend class Condition;
		friend class ReadWriteLock;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

