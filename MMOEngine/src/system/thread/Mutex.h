/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MUTEX_H_
#define MUTEX_H_

#include "Lockable.h"

namespace sys {
  namespace thread {

	class ReadWriteLock;

	class Mutex : public Lockable {
		pthread_mutex_t mutex;

	public:
		Mutex() : Lockable() {
			pthread_mutex_init(&mutex, NULL);
		}

		Mutex(const String& s) : Lockable(s) {
			pthread_mutex_init(&mutex, NULL);
		}

		virtual ~Mutex() {
			pthread_mutex_destroy(&mutex);
		}

		void lock(bool doLock = true);
		void lock(Mutex* m);
		void lock(Lockable* lockable);

		bool tryLock();
		bool tryLock(uint64 time);

		void unlock(bool doLock = true);

		friend class Condition;
		friend class ReadWriteLock;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*MUTEX_H_*/
