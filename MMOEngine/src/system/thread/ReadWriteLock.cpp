/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../lang/Time.h"

#include "Mutex.h"
#include "ReadWriteLock.h"

void ReadWriteLock::wlock(Mutex* lock) {
	lockAcquiring(lock, "w");

    while (pthread_rwlock_trywrlock(&rwlock)) {
    	#ifndef TRACE_LOCKS
			pthread_mutex_unlock(&(lock->mutex));
      		pthread_mutex_lock(&(lock->mutex));
		#else
      		lock->unlock();
	      	lock->lock();
	 	#endif
	}

	lockAcquired(lock, "w");
}

void ReadWriteLock::wlock(ReadWriteLock* lock) {
	if (this == lock) {
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross wlocking itself [" << lockName << "]\n";

		StackTrace::printStackTrace();
		return;
	}

	#ifdef TRACE_LOCKS
		if (lock->threadIDLockHolder == 0) {
			System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross wlocking to an unlocked mutex [" << lock->lockName << "]\n";
			StackTrace::printStackTrace();

			raise(SIGSEGV);
		}
	#endif

	lockAcquiring(lock, "w");

    while (pthread_rwlock_trywrlock(&rwlock)) {
    	#ifndef TRACE_LOCKS
  			pthread_rwlock_unlock(&(lock->rwlock));
       		pthread_rwlock_wrlock(&(lock->rwlock));
       	#else
       		lock->unlock();
       		lock->wlock();
       	#endif
		}

	lockAcquired(lock, "w");
}

void ReadWriteLock::lock(Lockable* lockable) {
	lockAcquiring(lockable, "w");

    while (pthread_rwlock_trywrlock(&rwlock)) {
  		lockable->unlock();
      	lockable->lock();
	}

	lockAcquired(lockable, "w");
}
