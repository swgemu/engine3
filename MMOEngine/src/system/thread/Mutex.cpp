/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/lang/Time.h"

#include "Mutex.h"
#include "engine/core/TaskWorkerThread.h"

void Mutex::lock(bool doLock) {
	if (!doLock)
		return;

	const auto start = lockAcquiring("w");

	#if !defined(TRACE_LOCKS) || defined(PLATFORM_CYGWIN)
		int res = pthread_mutex_lock(&mutex);

		if (res != 0)
	#ifdef TRACE_LOCKS
			System::out << "(" << Time::currentNanoTime() << " nsec) lock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";
	#else
			System::out << "(" << Time::currentNanoTime() << " nsec) lock() failed on Mutex (" << res << ")\n";
	#endif
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

	const auto end = lockAcquired("w");

	const auto diff = end - start;

#ifdef TRACE_BLOCK_TIME
	auto thread = Thread::getCurrentThread();

	if (!thread)
		return;

	auto worker = thread->asTaskWorkerThread();

	if (!worker)
		return;

	worker->addMutexWaitTime(diff);
#endif
}

void Mutex::lock(Mutex* m) {
	if (this == m) {
#ifdef TRACE_LOCKS
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross locking itself [" << lockName << "]\n";
#else
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross locking itself\n";
#endif

		StackTrace::printStackTrace();
		return;
	}

	const auto start = lockAcquiring(m, "w");
	auto currentThreadThread = m->getLockHolderThread();

	while (pthread_mutex_trylock(&mutex)) {
		m->clearCurrentLockHolder();

		pthread_mutex_unlock(&(m->mutex));

#ifdef ENABLE_YIELD_BETWEEN_CROSSLOCK
			Thread::yield();
#endif
		pthread_mutex_lock(&(m->mutex));

		m->setCurrentLockHolder(currentThreadThread);
	}

	const auto end = lockAcquired(m, "w");

	const auto diff = end - start;

#ifdef TRACE_BLOCK_TIME
	auto thread = Thread::getCurrentThread();

	if (!thread)
		return;

	auto worker = thread->asTaskWorkerThread();

	if (!worker)
		return;

	worker->addMutexWaitTime(diff);
#endif
}

void Mutex::lock(Lockable* lockable) {
	if (this == lockable) {
#ifdef TRACE_LOCKS
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross locking itself [" << lockName << "]\n";
#else
		System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross locking itself \n";
#endif

		StackTrace::printStackTrace();
		return;
	}

	const auto start = lockAcquiring(lockable, "w");

	while (pthread_mutex_trylock(&mutex)) {
		lockable->unlock();

#ifdef ENABLE_YIELD_BETWEEN_CROSSLOCK
		Thread::yield();
#endif
		lockable->lock();
	}

	const auto end = lockAcquired(lockable, "w");

	const auto diff = end - start;

#ifdef TRACE_BLOCK_TIME
	auto thread = Thread::getCurrentThread();

	if (!thread)
		return;

	auto worker = thread->asTaskWorkerThread();

	if (!worker)
		return;

	worker->addMutexWaitTime(diff);
#endif
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

	lockReleasing("w");

	int res = pthread_mutex_unlock(&mutex);
	if (res != 0) {
#ifdef TRACE_LOCKS
		System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";
#else
		System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on Mutex (" << res << ")\n";
#endif

		StackTrace::printStackTrace();
	}

	lockReleased("w");
}

