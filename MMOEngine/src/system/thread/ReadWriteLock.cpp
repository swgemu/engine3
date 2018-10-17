/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Mutex.h"

#include "ReadWriteLock.h"

void ReadWriteLock::rlock(bool doLock) ACQUIRE_SHARED() {
	if (!doLock)
		return;

	lockAcquiring("r");

	#if !defined(TRACE_LOCKS) || defined(PLATFORM_CYGWIN)
		int res = pthread_rwlock_rdlock(&rwlock);
		if (res != 0) {
#ifdef TRACE_LOCKS
			System::out << "(" << Time::currentNanoTime() << " nsec) rlock() failed on RWLock \'" << lockName << "\' (" << strerror(res) << ")\n";
#else
			System::out << "(" << Time::currentNanoTime() << " nsec) rlock() failed on RWLock (" << strerror(res) << ")\n";
#endif
			StackTrace::printStackTrace();
			
			assert(0 && "rlock failed");
		}
	#else
		#ifndef LOG_LOCKS
			int cnt = lockCount.increment();
		#endif

		Time start;
		start.addMiliTime(10000);

		while (pthread_rwlock_timedrdlock(&rwlock, start.getTimeSpec())) {
			if (!doTrace)
				continue;

			traceDeadlock("r");

			start.addMiliTime(1000);
		}

		lockTime->updateToCurrentTime();
	#endif

	readLockCount.increment();

	lockAcquired("r");
}

void ReadWriteLock::wlock(bool doLock) ACQUIRE() {
	if (!doLock)
		return;

	lockAcquiring("w");

	#if !defined(TRACE_LOCKS) || defined(PLATFORM_CYGWIN)
		int res = pthread_rwlock_wrlock(&rwlock);
		if (res != 0){
#ifdef TRACE_LOCKS
			System::out << "(" << Time::currentNanoTime() << " nsec) wlock() failed on RWLock \'" << lockName << "\' (" << strerror(res) << ")\n";
#else
		System::out << "(" << Time::currentNanoTime() << " nsec) wlock() failed on RWLock (" << strerror(res) << ")\n";
#endif
		assert(0 && "wlock failed");
		}
	#else
		Time start;
		start.addMiliTime(10000);

		int result = 0;

		while ((result = pthread_rwlock_timedwrlock(&rwlock, start.getTimeSpec())) != 0) {
			if (result != ETIMEDOUT) {
				System::out << "(" << Time::currentNanoTime() << " nsec) wlock() error on RWLock \'" << lockName << "\' (" << strerror(result) << ")\n";

				raise(SIGSEGV);
			}

			if (!doTrace)
				continue;

			traceDeadlock("w");

			start.addMiliTime(1000);
		}

		lockTime->updateToCurrentTime();
	#endif

	lockAcquired("w");
}

void ReadWriteLock::wlock(Mutex* lock) ACQUIRE() {
	lockAcquiring(lock, "w");

    while (pthread_rwlock_trywrlock(&rwlock)) {
    	#ifndef TRACE_LOCKS
			//pthread_mutex_unlock(&(lock->mutex));

    		lock->unlock();

#ifdef ENABLE_YIELD_BETWEEN_CROSSLOCK
			Thread::yield();
#endif
      		//pthread_mutex_lock(&(lock->mutex));

			lock->lock();
		#else
      		lock->unlock();

      		Thread::yield();

	      	lock->lock();
	 	#endif
	}

	lockAcquired(lock, "w");
}

void ReadWriteLock::rlock(Lockable* lock) ACQUIRE_SHARED() {
	lockAcquiring(lock, "r");

	while (pthread_rwlock_tryrdlock(&rwlock)) {
		lock->unlock();

#ifdef ENABLE_YIELD_BETWEEN_CROSSLOCK
		Thread::yield();
#endif

		lock->lock();
	}

	readLockCount.increment();

	lockAcquired(lock, "r");
}

void ReadWriteLock::rlock(ReadWriteLock* lock) ACQUIRE_SHARED() {
	lockAcquiring(lock, "r");

	while (pthread_rwlock_tryrdlock(&rwlock)) {
		lock->unlock();

#ifdef ENABLE_YIELD_BETWEEN_CROSSLOCK
		Thread::yield();
#endif

		lock->lock();
	}

	readLockCount.increment();

	lockAcquired(lock, "r");
}

void ReadWriteLock::wlock(ReadWriteLock* lock) ACQUIRE() {
	if (this == lock) {
#ifdef TRACE_LOCKS
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross wlocking itself [" << lockName << "]\n";
#else
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross wlocking itself \n";
#endif

		StackTrace::printStackTrace();
		return;
	}

	#ifdef TRACE_LOCKS
		if (lock->threadLockHolder == nullptr) {
			System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross wlocking to an unlocked mutex [" << lock->lockName << "]\n";
			StackTrace::printStackTrace();

			raise(SIGSEGV);
		}
	#endif

	lockAcquiring(lock, "w");

    while (pthread_rwlock_trywrlock(&rwlock)) {
    	#ifndef TRACE_LOCKS
  			//pthread_rwlock_unlock(&(lock->rwlock));
    		lock->unlock();

#ifdef ENABLE_YIELD_BETWEEN_CROSSLOCK
  			Thread::yield();
#endif
       		//pthread_rwlock_wrlock(&(lock->rwlock));
  			lock->wlock();
       	#else
       		lock->unlock();
       		lock->wlock();
       	#endif
	}

	lockAcquired(lock, "w");
}

void ReadWriteLock::lock(Lockable* lockable) ACQUIRE() {
	lockAcquiring(lockable, "w");

    while (pthread_rwlock_trywrlock(&rwlock)) {
  		lockable->unlock();

#ifdef ENABLE_YIELD_BETWEEN_CROSSLOCK
  		Thread::yield();
#endif
      	lockable->lock();
	}

	lockAcquired(lockable, "w");
}

void ReadWriteLock::unlock(bool doLock) RELEASE() {
	if (!doLock)
		return;

	#if defined(TRACE_LOCKS) && !defined(PLATFORM_CYGWIN)
		if (threadLockHolder == nullptr) {
			System::out << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "]"
					<< " unlocking an unlocked mutex\n";
			StackTrace::printStackTrace();

			if (unlockTrace != nullptr) {
				System::out << "previously unlocked by\n";
				unlockTrace->print();
			}

			raise(SIGSEGV);
		} else if (threadLockHolder != Thread::getCurrentThread()) {
			System::out << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "]" << " mutex unlocked by a different thread\n";
			StackTrace::printStackTrace();

			if (trace != nullptr) {
				System::out << "previously locked at " << lockTime->getMiliTime() << " by\n";
				trace->print();
			}
		}
	#endif
	
//	assert(threadLockHolder == Thread::getCurrentThread());

	lockReleasing();

	int res = pthread_rwlock_unlock(&rwlock);
	if (res != 0) {
#ifdef TRACE_LOCKS
		System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on RWLock \'" << lockName << "\' (" << strerror(res) << ")\n";
#else
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross wlocking itself\n";
#endif

		StackTrace::printStackTrace();
		assert(0 && "unlock failed");
	}

	lockReleased();
}

void ReadWriteLock::runlock(bool doLock) RELEASE_SHARED() {
	if (!doLock)
		return;

#ifdef TRACE_LOCKS
	/*if (threadIDLockHolder == 0) {
					System::out << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "]"
							<< " unlocking an unlocked mutex\n";
					StackTrace::printStackTrace();
				} else if (threadIDLockHolder != Thread::getCurrentThreadID()) {
					System::out << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "]" << " mutex unlocked by a different thread\n";
					StackTrace::printStackTrace();

					if (trace != nullptr) {
						System::out << "previously locked at " << lockTime->getMiliTime() << " by\n";
						trace->print();
					}
				}*/

	/*delete trace;
	trace = nullptr;

	threadIDLockHolder = 0;*/
#endif

	lockReleasing("r");

	readLockCount.decrement();

	int res = pthread_rwlock_unlock(&rwlock);
	if (res != 0) {
#ifdef TRACE_LOCKS
		System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on RWLock \'" << lockName << "\' (" << res << ")\n";
#else
		System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on RWLock (" << res << ")\n";
#endif

		StackTrace::printStackTrace();
		
		assert(0 && "runlock failed");
	}

	lockReleased();
}
