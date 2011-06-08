/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../lang/Time.h"

#include "Mutex.h"
#include "../lang/System.h"


void Mutex::lock(bool doLock) {
	if (!doLock)
		return;

	lockAcquiring();

	#if !defined(TRACE_LOCKS) || defined(PLATFORM_CYGWIN)
		int res = pthread_mutex_lock(&mutex);

		if (res != 0)
			System::out << "(" << Time::currentNanoTime() << " nsec) lock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";
	#else
		Time start;
		start.addMiliTime(10000);

		while (pthread_mutex_timedlock(&mutex, start.getTimeSpec())) {
			if (!doTrace)
				continue;

			traceDeadlock();

			start.addMiliTime(1000);
		}

		lockTime->updateToCurrentTime();
	#endif

	lockAcquired();
}

void Mutex::lock(Mutex* m) {
	if (this == m) {
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross locking itself [" << lockName << "]\n";

		StackTrace::printStackTrace();
		return;
	}

	lockAcquiring(m);

	while (pthread_mutex_trylock(&mutex)) {
		#ifndef TRACE_LOCKS
			pthread_mutex_unlock(&(m->mutex));
			pthread_mutex_lock(&(m->mutex));
		#else
			m->unlock();
			m->lock();
		#endif
	}

	lockAcquired(m);
}

void Mutex::lock(Lockable* lockable) {
	if (this == lockable) {
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross locking itself [" << lockName << "]\n";

		StackTrace::printStackTrace();
		return;
	}

	lockAcquiring(lockable);

	while (pthread_mutex_trylock(&mutex)) {
		lockable->unlock();
		lockable->lock();
	}

	lockAcquired(lockable);
}

bool Mutex::tryLock() {
	return pthread_mutex_trylock(&mutex) == 0;
}

#ifndef PLATFORM_MAC
bool Mutex::tryLock(uint64 millis) {
	Time timeout;
	timeout.addMiliTime(millis);

	return pthread_mutex_timedlock(&mutex, timeout.getTimeSpec()) == 0;
}
#endif

void Mutex::unlock(bool doLock) {
	if (!doLock)
		return;

	lockReleasing();

	int res = pthread_mutex_unlock(&mutex);
	if (res != 0) {
		System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";

		StackTrace::printStackTrace();
	}

	lockReleased();
}
