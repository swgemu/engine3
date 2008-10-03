/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../lang/Time.h"

#include "Mutex.h"
#include "ReadWriteLock.h"

void ReadWriteLock::wlock(Mutex* lock) {
	#ifdef LOG_LOCKS
		Atomic::incrementInt((uint32*)&lockCount);
		int cnt = lockCount;
		cout << "[" << lockName << "] acquiring cross mutex wlock #" << cnt << "\n";
	#endif

    while (pthread_rwlock_trywrlock(&rwlock)) {
    	#ifndef TRACE_LOCKS
			pthread_mutex_unlock(&(lock->mutex));
      		pthread_mutex_lock(&(lock->mutex));
		#else
      		lock->unlock();
	      	lock->lock();
	 	#endif
	}

	#ifdef TRACE_LOCKS
		threadIDLockHolder = Thread::getCurrentThreadID();

		if (doTrace) {
			delete trace;
			trace = new StackTrace();
		}
	#endif

	#ifdef LOG_LOCKS
		cout << "[" << lockName << "] acquired cross mutex wlock #" << cnt << "\n";
	#endif
}

void ReadWriteLock::wlock(ReadWriteLock* lock) {
	if (this == lock) {
		cout << "(" << Time::currentNanoTime() << " nsec) ERROR: cross wlocking itself [" << lockName << "]\n";

		StackTrace::printStackTrace();
		return;
	}

	#ifdef TRACE_LOCKS
		if (lock->threadIDLockHolder == 0) {
			cout << "(" << Time::currentNanoTime() << " nsec) ERROR: cross wlocking to an unlocked mutex [" << lock->lockName << "]\n";
			StackTrace::printStackTrace();
		}
	#endif

	#ifdef LOG_LOCKS
		Atomic::incrementInt((uint32*)&lockCount);
		int cnt = lockCount;
		cout << "(" << Time::currentNanoTime() << " nsec) [" << lockName << " (" << lock->lockName << ")] acquiring cross wlock #" << cnt << "\n";
	#endif

    while (pthread_rwlock_trywrlock(&rwlock)) {
    	#ifndef TRACE_LOCKS
  			pthread_rwlock_unlock(&(lock->rwlock));
       		pthread_rwlock_wrlock(&(lock->rwlock));
       	#else
       		lock->unlock();
       		lock->wlock();
       	#endif
		}

	#ifdef TRACE_LOCKS
		threadIDLockHolder = Thread::getCurrentThreadID();

		if (doTrace) {
			delete trace;
			trace = new StackTrace();
		}
	#endif

	#ifdef LOG_LOCKS
		cout << "(" << Time::currentNanoTime() << " nsec) [" << lockName << " (" << lock->lockName << ")] acquired cross wlock #" << cnt << "\n";
	#endif
}
