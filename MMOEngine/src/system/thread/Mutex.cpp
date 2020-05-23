/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/lang/Time.h"

#include "Mutex.h"
#include "engine/core/TaskWorkerThread.h"

void Mutex::lock(bool doLock, const char* file, const char* function, int line) {
	if (!doLock)
		return;

	const auto start = lockAcquiring("w", file, function, line);

#ifdef TRACK_LOCKS

	Time tryUntil;
	tryUntil.addMiliTime(TRACK_LOCKS_INITIAL_TIMEOUT);

	int lockResult = 0;
	int delay = TRACK_LOCKS_RETRY_BASE;

	while ((lockResult = pthread_mutex_timedlock(&mutex, tryUntil.getTimeSpec())) == ETIMEDOUT) {
		traceDeadlock("w", file, function, line);

		tryUntil.addMiliTime(delay + System::random(TRACK_LOCKS_RETRY_JITTER));
		delay = delay < 60000 ? delay + delay : 60000;
	}

#else // not TRACK_LOCKS

	int lockResult = pthread_mutex_lock(&mutex);

#endif // TRACK_LOCKS

	const auto end = lockAcquired("w", file, function, line);

#ifdef TRACE_BLOCK_TIME
	auto thread = Thread::getCurrentThread();

	if (!thread)
		return;

	auto worker = thread->asTaskWorkerThread();

	if (!worker)
		return;

	worker->addMutexWaitTime(end - start);
#endif
}

void Mutex::lock(Mutex* m, const char* file, const char* function, int line) {
	const auto start = lockAcquiring(m, "w", file, function, line);

	auto currentLockHolder = m->getLockHolderThread();

#ifdef TRACK_LOCKS

	Time tryUntil;

	int lockResult = 0;
	int delay = TRACK_LOCKS_RETRY_BASE;

	while ((lockResult = pthread_mutex_trylock(&mutex)) != 0) {
		m->clearCurrentLockHolder("w", currentLockHolder);

		pthread_mutex_unlock(&(m->mutex));

		CROSSLOCK_YIELD();

		tryUntil.addMiliTime(TRACK_LOCKS_INITIAL_TIMEOUT);

		while ((lockResult = pthread_mutex_timedlock(&(m->mutex), tryUntil.getTimeSpec())) == ETIMEDOUT) {
			traceDeadlock("w", file, function, line);

			tryUntil.addMiliTime(delay + System::random(TRACK_LOCKS_RETRY_JITTER));
			delay = delay < 60000 ? delay + delay : 60000;
		}

		if (lockResult != 0) {
			m->logLockResult("w", currentLockHolder, file, function, line, lockResult);
			assert(0 && "cross wlock Mutex failed");
		}

		m->setCurrentLockHolder("w", currentLockHolder, file, function, line);
	}

#else // not TRACK_LOCKS

	while (pthread_mutex_trylock(&mutex)) {
		m->clearCurrentLockHolder("w", currentLockHolder);

		pthread_mutex_unlock(&(m->mutex));

		CROSSLOCK_YIELD();

		pthread_mutex_lock(&(m->mutex));

		m->setCurrentLockHolder("w", currentLockHolder, file, function, line);
	}

#endif // TRACK_LOCKS

	const auto end = lockAcquired(m, "w", file, function, line);

#ifdef TRACE_BLOCK_TIME
	auto thread = Thread::getCurrentThread();

	if (!thread)
		return;

	auto worker = thread->asTaskWorkerThread();

	if (!worker)
		return;

	worker->addMutexWaitTime(end - start);
#endif
}

void Mutex::lock(Lockable* lockable, const char* file, const char* function, int line) {
	if (this == lockable) {
		logLockError("w", "ERROR - mutext cross locking with itself", file, function, line);
		return;
	}

	const auto start = lockAcquiring(lockable, "w", file, function, line);

	while (pthread_mutex_trylock(&mutex)) {
		lockable->unlock();

		CROSSLOCK_YIELD();

		lockable->lock();
	}

	const auto end = lockAcquired(lockable, "w", file, function, line);

#ifdef TRACE_BLOCK_TIME
	auto thread = Thread::getCurrentThread();

	if (!thread)
		return;

	auto worker = thread->asTaskWorkerThread();

	if (!worker)
		return;

	worker->addMutexWaitTime(end - start);
#endif
}

bool Mutex::tryLock(const char* file, const char* function, int line) {
	lockAcquiring("w", file, function, line);

	if (pthread_mutex_trylock(&mutex) == 0) {
		lockAcquired("w", file, function, line);
		return true;
	}

	return false;
}

#ifndef PLATFORM_MAC
bool Mutex::tryLock(uint64 millis, const char* file, const char* function, int line) {
	Time timeout;
	timeout.addMiliTime(millis);

	lockAcquiring("w", file, function, line);

	if (pthread_mutex_timedlock(&mutex, timeout.getTimeSpec()) == 0) {
		lockAcquired("w", file, function, line);
		return true;
	}

	return false;
}
#endif

void Mutex::unlock(bool doLock, const char* file, const char* function, int line) {
	if (!doLock)
		return;

	lockReleasing("w", file, function, line);

	int unlockResult = pthread_mutex_unlock(&mutex);

	if (unlockResult != 0) {
		logLockResult("w", file, function, line, unlockResult);
		assert(0 && "Mutex::unlock failed");
	}

	lockReleased("w", file, function, line);
}
