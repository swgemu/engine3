/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "Mutex.h"

#include "ReadWriteLock.h"

#include "engine/core/TaskWorkerThread.h"

void ReadWriteLock::rlock(bool doLock, const char* file, const char* function, int line) ACQUIRE_SHARED() {
	if (!doLock)
		return;

	const auto start = lockAcquiring("r", file, function, line);

#ifdef TRACK_LOCKS

	Time tryUntil;
	tryUntil.addMiliTime(TRACK_LOCKS_INITIAL_TIMEOUT);

	int lockResult = 0;
	int delay = TRACK_LOCKS_RETRY_BASE;

	while ((lockResult = pthread_rwlock_timedrdlock(&rwlock, tryUntil.getTimeSpec())) == ETIMEDOUT) {
		traceDeadlock("r", file, function, line);

		tryUntil.addMiliTime(delay + System::random(TRACK_LOCKS_RETRY_JITTER));
		delay = delay < 60000 ? delay + delay : 60000;
	}

#else // not TRACK_LOCKS

	int lockResult = pthread_rwlock_rdlock(&rwlock);

#endif // TRACK_LOCKS

	if (lockResult != 0) {
		logLockResult("r", file, function, line, lockResult);
		assert(0 && "ReadWriteLock::rlock failed");
	}

	const auto end = lockAcquired("r", file, function, line);

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

void ReadWriteLock::wlock(bool doLock, const char* file, const char* function, int line) ACQUIRE() {
	if (!doLock)
		return;

	const auto start = lockAcquiring("w", file, function, line);

#ifdef TRACK_LOCKS

	Time tryUntil;
	tryUntil.addMiliTime(TRACK_LOCKS_INITIAL_TIMEOUT);

	int lockResult = 0;
	int delay = TRACK_LOCKS_RETRY_BASE;

	while ((lockResult = pthread_rwlock_timedwrlock(&rwlock, tryUntil.getTimeSpec())) == ETIMEDOUT) {
		traceDeadlock("w", file, function, line);

		tryUntil.addMiliTime(delay + System::random(TRACK_LOCKS_RETRY_JITTER));
		delay = delay < 60000 ? delay + delay : 60000;
	}

#else // not TRACK_LOCKS

	int lockResult = pthread_rwlock_wrlock(&rwlock);

#endif // TRACK_LOCKS

	if (lockResult != 0) {
		logLockResult("w", file, function, line, lockResult);
		assert(0 && "ReadWriteLock::wlock failed");
	}

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

void ReadWriteLock::wlock(Mutex* lock, const char* file, const char* function, int line) ACQUIRE() {
	const auto start = lockAcquiring(lock, "w", file, function, line);

	auto currentLockHolder = lock->getLockHolderThread();

#ifdef TRACK_LOCKS

	Time tryUntil;

	int lockResult = 0;
	int delay = TRACK_LOCKS_RETRY_BASE;

	while ((lockResult = pthread_rwlock_trywrlock(&rwlock)) != 0) {
		lock->clearCurrentLockHolder("w", currentLockHolder);

		pthread_mutex_unlock(&(lock->mutex));

		CROSSLOCK_YIELD();

		tryUntil.addMiliTime(TRACK_LOCKS_INITIAL_TIMEOUT);

		while ((lockResult = pthread_mutex_timedlock(&(lock->mutex), tryUntil.getTimeSpec())) == ETIMEDOUT) {
			traceDeadlock("w", file, function, line);

			tryUntil.addMiliTime(delay + System::random(TRACK_LOCKS_RETRY_JITTER));
			delay = delay < 60000 ? delay + delay : 60000;
		}

		if (lockResult != 0) {
			lock->logLockResult("w", currentLockHolder, file, function, line, lockResult);
			assert(0 && "ReadWriteLock::cross wlock Mutex failed");
		}

		lock->setCurrentLockHolder("w", currentLockHolder, file, function, line);
	}

#else // not TRACK_LOCKS

	while (pthread_rwlock_trywrlock(&rwlock)) {
		lock->clearCurrentLockHolder("w", currentLockHolder);

		pthread_mutex_unlock(&(lock->mutex));

		CROSSLOCK_YIELD();

		pthread_mutex_lock(&(lock->mutex));

		lock->setCurrentLockHolder("w", currentLockHolder, file, function, line);
	}

#endif // TRACK_LOCKS

	const auto end = lockAcquired(lock, "w", file, function, line);

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

void ReadWriteLock::rlock(Lockable* lock, const char* file, const char* function, int line) ACQUIRE_SHARED() {
	const auto start = lockAcquiring(lock, "r", file, function, line);

	while (pthread_rwlock_tryrdlock(&rwlock)) {
		lock->unlock(true, file, function, line);

		CROSSLOCK_YIELD();

		lock->lock(true, file, function, line);
	}

	const auto end = lockAcquired(lock, "r", file, function, line);

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

void ReadWriteLock::rlock(ReadWriteLock* lock, const char* file, const char* function, int line) ACQUIRE_SHARED() {
	const auto start = lockAcquiring(lock, "r", file, function, line);

	while (pthread_rwlock_tryrdlock(&rwlock)) {
		lock->unlock(true, file, function, line);

		CROSSLOCK_YIELD();

		lock->lock(true, file, function, line);
	}

	const auto end = lockAcquired(lock, "r", file, function, line);

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

void ReadWriteLock::wlock(ReadWriteLock* lock, const char* file, const char* function, int line) ACQUIRE() {
	if (this == lock) {
		logLockError("w", "ERROR - cross locking with itself", file, function, line);
		return;
	}

	const auto start = lockAcquiring(lock, "w", file, function, line);

	auto currentLockHolder = lock->getLockHolderThread();

#ifdef TRACK_LOCKS

	Time tryUntil;

	int lockResult = 0;
	int delay = TRACK_LOCKS_RETRY_BASE;

	while ((lockResult = pthread_rwlock_trywrlock(&rwlock)) != 0) {
		lock->clearCurrentLockHolder("w", currentLockHolder);

		pthread_rwlock_unlock(&(lock->rwlock));

		CROSSLOCK_YIELD();

		tryUntil.addMiliTime(TRACK_LOCKS_INITIAL_TIMEOUT);

		while ((lockResult = pthread_rwlock_timedwrlock(&(lock->rwlock), tryUntil.getTimeSpec())) == ETIMEDOUT) {
			traceDeadlock("w", file, function, line);

			tryUntil.addMiliTime(delay + System::random(TRACK_LOCKS_RETRY_JITTER));
			delay = delay < 60000 ? delay + delay : 60000;
		}

		if (lockResult != 0) {
			lock->logLockResult("w", currentLockHolder, file, function, line, lockResult);
			assert(0 && "ReadWriteLock::cross wlock Mutex failed");
		}

		lock->setCurrentLockHolder("w", currentLockHolder, file, function, line);
	}

#else // not TRACK_LOCKS

	while (pthread_rwlock_trywrlock(&rwlock)) {
		lock->clearCurrentLockHolder("w", currentLockHolder);

		pthread_rwlock_unlock(&(lock->rwlock));

		CROSSLOCK_YIELD();

		pthread_rwlock_wrlock(&(lock->rwlock));

		lock->setCurrentLockHolder("w", currentLockHolder, file, function, line);
	}

#endif // TRACK_LOCKS

	const auto end = lockAcquired(lock, "w", file, function, line);

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

void ReadWriteLock::lock(Lockable* lockable, const char* file, const char* function, int line) ACQUIRE() {
	const auto start = lockAcquiring(lockable, "w", file, function, line);

	while (pthread_rwlock_trywrlock(&rwlock)) {
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

void ReadWriteLock::unlock(bool doLock, const char* file, const char* function, int line) RELEASE() {
	if (!doLock)
		return;

	lockReleasing("w", file, function, line);

	int unlockResult = pthread_rwlock_unlock(&rwlock);

	if (unlockResult != 0) {
		logLockResult("r", file, function, line, unlockResult);
		assert(0 && "ReadWriteLock::unlock failed");
	}

	lockReleased("w", file, function, line);
}

void ReadWriteLock::runlock(bool doLock, const char* file, const char* function, int line) RELEASE_SHARED() {
	if (!doLock)
		return;

	lockReleasing("r", file, function, line);

	int unlockResult = pthread_rwlock_unlock(&rwlock);

	if (unlockResult != 0) {
		logLockResult("r", file, function, line, unlockResult);
		assert(0 && "ReadWriteLock::runlock failed");
	}

	lockReleased("r", file, function, line);
}
