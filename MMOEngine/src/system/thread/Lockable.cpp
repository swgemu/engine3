/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/lang/Time.h"

#include "Thread.h"

#include "Lockable.h"

#include <stdlib.h>

#ifdef TRACK_LOCKS

#include "system/io/FileWriter.h"
#include "system/util/SortedVector.h"

static_assert(TRACK_LOCKS_MAX_HISTORY >= 2, "TRACK_LOCKS_MAX_HISTORY must be >= 2");

#define TRACK_LOCKS_LOG_ACTION(modifier, file, func, line) if (mLockLogFile != nullptr) { logLockAction(__FUNCTION__, modifier, file, func, line); }
#else // not TRACK_LOCKS
#define TRACK_LOCKS_LOG_ACTION(modifier, file, func, line)
#endif // TRACK_LOCKS

#ifdef TRACE_BLOCK_TIME
#define TRACE_BLOCK_TIME_RETURN Time::currentNanoTime(Time::MONOTONIC_TIME);
#else // not TRACE_BLOCK_TIME
#define TRACE_BLOCK_TIME_RETURN 0 // Don't waste time getting time!
#endif // TRACE_BLOCK_TIME

Lockable::Lockable() {
	threadLockHolder.set(nullptr);
}

Lockable::Lockable(const String& name) : Lockable() {
	setLockName(name);
}

Lockable::~Lockable() {
#ifdef TRACK_LOCKS

	if (mLockName != nullptr) {
		free((void*)mLockName);
		mLockName = nullptr;
	}

	if (mLockLogFile != nullptr) {
		setLockLogging(false);
	}

#endif // TRACK_LOCKS
}

uint64 Lockable::lockAcquiring(const char* modifier, const char* file, const char* function, int line) {
	TRACK_LOCKS_LOG_ACTION(modifier, file, function, line);

	return TRACE_BLOCK_TIME_RETURN;
}

uint64 Lockable::lockAcquiring(Lockable* lockable, const char* modifier, const char* file, const char* function, int line) {
	TRACK_LOCKS_LOG_ACTION(modifier, file, function, line);

	return TRACE_BLOCK_TIME_RETURN;
}

uint64 Lockable::lockAcquired(const char* modifier, const char* file, const char* function, int line) {
	setCurrentLockHolder(modifier, Thread::getCurrentThread(), file, function, line);

	TRACK_LOCKS_LOG_ACTION(modifier, file, function, line);

	return TRACE_BLOCK_TIME_RETURN;
}

uint64 Lockable::lockAcquired(Lockable* lockable, const char* modifier, const char* file, const char* function, int line) {
	setCurrentLockHolder(modifier, Thread::getCurrentThread(), file, function, line);

	TRACK_LOCKS_LOG_ACTION(modifier, file, function, line);

	return TRACE_BLOCK_TIME_RETURN;
}

void Lockable::lockReleasing(const char* modifier, const char* file, const char* function, int line) {
#ifdef TRACK_LOCKS

	if (modifier[0] != 'r') {
		auto thread = threadLockHolder.get();

	#ifdef TRACK_LOCKS_REPORT_DOUBLE_UNLOCK

		if (thread == nullptr && Thread::getCurrentThread() != nullptr && file != nullptr && strstr(file, "MMOEngine/") == nullptr) {
			logLockError(modifier, "Unlocking lock that is already unlocked", file, function, line);
			__asm__("int $3");
		}

	#endif // TRACK_LOCKS_REPORT_DOUBLE_UNLOCK

	#ifdef TRACK_LOCKS_REPORT_CROSS_UNLOCK

		if (thread != nullptr && thread != Thread::getCurrentThread()) {
			logLockError(modifier, "Unlocking lock owned by [Thread " + String::valueOf(safeGetThreadNumber(thread)) + "]", file, function, line);
		}

	#endif // TRACK_LOCKS_REPORT_CROSS_UNLOCK

	}

#endif // TRACK_LOCKS

	clearCurrentLockHolder(modifier, Thread::getCurrentThread());

	TRACK_LOCKS_LOG_ACTION(modifier, file, function, line);
}

void Lockable::lockReleased(const char* modifier, const char* file, const char* function, int line) {
	TRACK_LOCKS_LOG_ACTION(modifier, file, function, line);
}

void Lockable::clearCurrentLockHolder(const char* modifier, Thread* thread) {
	if (modifier[0] != 'r') {
		threadLockHolder.set(nullptr, std::memory_order_relaxed);
	}

#ifdef TRACK_LOCKS

	std::unique_lock<std::mutex> guard(mGuardMutex);

#if TRACK_LOCKS_MAX_HISTORY == 2
	if (modifier[0] != 'r') {
		mLockHolders[0].clear();
		return;
	}

	if (mLockHolders[1].getThreadNumber() == safeGetThreadNumber(thread)) {
		mLockHolders[1].clear();
	}

#else // TRACK_LOCKS_MAX_HISTORY != 2

	bool isWrite = modifier[0] != 'r';
	auto threadNumber = safeGetThreadNumber(thread);

	for (int i = 0; i < TRACK_LOCKS_MAX_HISTORY; ++i) {
		if (!mLockHolders[i].isSet() || mLockHolders[i].isWrite() != isWrite) {
			continue;
		}

		if (mLockHolders[i].getThreadNumber() == threadNumber) {
			mLockHolders[i].clear();
			return;
		}
	}

#endif // TRACK_LOCKS_MAX_HISTORY

#endif // TRACK_LOCKS
}

void Lockable::setCurrentLockHolder(const char* modifier, Thread*  thread, const char* file, const char* function, int line) {
	if (modifier[0] != 'r') {
		threadLockHolder.set(thread, std::memory_order_relaxed);
	}

#ifdef TRACK_LOCKS
	std::unique_lock<std::mutex> guard(mGuardMutex);

#if TRACK_LOCKS_MAX_HISTORY == 2

	if (modifier[0] != 'r') {
		mLockHolders[0].set(modifier, safeGetThreadNumber(thread), file, function, line);
		return;
	}

	if (mLockHolders[1].isSet()) {
		return;
	}

	mLockHolders[1].set(modifier, safeGetThreadNumber(thread), file, function, line);

#else // TRACK_LOCKS_MAX_HISTORY != 2
	uint64 oldestTime = -1;
	auto oldestEntry = 0;

	for (int i = 0; i < TRACK_LOCKS_MAX_HISTORY; ++i) {
		auto holder = &mLockHolders[i];

		if (!holder->wasSet()) {
			holder->set(modifier, safeGetThreadNumber(thread), file, function, line);
			return;
		}

		if (holder->isSet() && holder->isWrite()) {
			continue;
		}

		auto thisSequence = holder->getLockSequence();

		if (thisSequence < oldestTime) {
			oldestEntry = i;
			oldestTime = thisSequence;
		}
	}

	mLockHolders[oldestEntry].set(modifier, safeGetThreadNumber(thread), file, function, line);
#endif

#endif // TRACK_LOCKS
}

void Lockable::logLockResult(const char* modifier, Thread* thread, const char* file, const char* function, int line, int lockResult) const {
	StringBuffer buf;

	buf << "WARNING - Lock failed with " << strerror(lockResult);

#ifdef TRACK_LOCKS

	logLockError(modifier, buf.toString(), thread, file, function, line);

#else // TRACK_LOCKS

	Time now;
	auto threadName = thread != nullptr ? thread->getName() : "<nullptr thread>";

	System::out
		<< now.getFormattedTime() << " " << threadName << " " << getLockName()
		<< buf.toString()
		<< " from " << file << ":" << line << " " << function << "()\n";

#endif // TRACK_LOCKS
}

#ifdef TRACK_LOCKS

const String Lockable::getLockHolderDetails() const {
	std::unique_lock<std::mutex> guard(mGuardMutex);

	StringBuffer buf;

#if TRACK_LOCKS_MAX_HISTORY == 2
	if (mLockHolders[0].wasSet()) {
		buf << (mLockHolders[0].isSet() ? " ACTIVE   " : " released ") << mLockHolders[0].format() << "\n";
	}

	if (mLockHolders[1].wasSet()) {
		buf << (mLockHolders[1].isSet() ? " ACTIVE   " : " released ") << mLockHolders[1].format() << "\n";
	}

#else // TRACK_LOCKS_MAX_HISTORY != 2

	SortedVector<LockHolder> locks;

	for (int i = 0; i < TRACK_LOCKS_MAX_HISTORY; ++i) {
		if (mLockHolders[i].wasSet()) {
			locks.put(mLockHolders[i]);
		}
	}

	for (int i = locks.size() - 1;i >= 0; --i) {
		auto holder = locks.get(i);
		buf << (holder.isSet() ? " ACTIVE   " : " released ") << holder.format() << "\n";
	}
#endif

	return buf.toString();
}

const String Lockable::getLockName() const {
	StringBuffer buf;

	buf << "[Lockable " << this;

	if (mLockName != nullptr) {
		buf << "; " << mLockName;
	}

	if (mTargetOID != 0) {
		buf << "; oid: " << mTargetOID;
	}

	buf << "]";

	return buf.toString();
}

void Lockable::logLockMessage(const String& msg) const {
	Time now;
	StringBuffer buf;

	buf << now.getFormattedTimeFull() << " [Thread " << safeGetThreadNumber(Thread::getCurrentThread()) << "] " << getLockName() << " " << msg;

	auto logEntry = buf.toString();

	System::out << "\033[41;30m" << logEntry << "\033[0m\n";

	if (mLockLogFile != nullptr) {
		writeLockLog(logEntry, true);
	}
}

void Lockable::logLockError(const char* modifier, const String& msg, Thread* thread, const char* file, const char* function, int line, bool fatal) const {
	StringBuffer buf;

	buf << msg << ":\n" << " LOCKERR  ";

	buf << LockHolder::format(modifier[0], safeGetThreadNumber(thread), file, function, line);

	buf << "\n" << getLockHolderDetails();

	logLockMessage(buf.toString());

	if (fatal) {
		StackTrace::printStackTrace();

		logLockMessage("FATAL LOCK ERROR: " + msg);

		assert(0 && "fatal lock error");
	}
}

void Lockable::writeLockLog(const String& msg, bool writeRaw) const {
	auto logFile = mLockLogFile.get();

	if (logFile == nullptr) {
		return;
	}

	static std::mutex writeMutex;
	Time now;
	StringBuffer buf;

	if (!writeRaw) {
		buf << now.getFormattedTimeFull() << " [Thread " << safeGetThreadNumber(Thread::getCurrentThread()) << "] " << getLockName() << " ";
	}

	buf << msg << "\n";

	(*logFile) << buf.toString();
	logFile->flush();
}

void Lockable::logLockAction(const char* method, const char* modifier, const char* file, const char* func, int line) const {
	if (mLockLogFile == nullptr) {
		return;
	}

	StringBuffer buf;
	buf << method << " " << LockHolder::format(modifier[0], safeGetThreadNumber(Thread::getCurrentThread()), file, func, line);

	writeLockLog(buf.toString());
}

void Lockable::traceDeadlock(const char* modifier, const char* file, const char* function, int line) {
	setLockLogging(true);
	logLockError(modifier, "WARNING - Lock Timeout", file, function, line);
}

void Lockable::setLockName(const String& name) {
	if (mLockName != nullptr) {
		free((void*)mLockName);
	}

	mLockName = strdup(name.toCharArray());
}

void Lockable::setTargetOID(uint64 oid) {
	mTargetOID = oid;
}

void Lockable::setLockLogging(bool doLog) {
	if (doLog == (mLockLogFile != nullptr)) {
		return;
	}

	if (!doLog) {
		auto logFile = mLockLogFile.get();

		if (logFile != nullptr && mLockLogFile.compareAndSet(logFile, nullptr)) {
			logFile->close();
			delete logFile->getFile();
			delete logFile;
		}

		return;
	}

	StringBuffer fileName;
	fileName << "log/lockLog-" << Thread::getProcessID() << "-" << hex << this << ".log";

	mLockLogFile.set(new FileWriter(new File(fileName.toString()), true));

	StringBuffer buf;
	buf << "Opened, holder history:\n" << getLockHolderDetails();
	writeLockLog(buf.toString());
}

Lockable& Lockable::operator=(const Lockable & ref) {
	if (this == &ref) {
		return *this;
	}

	std::unique_lock<std::mutex> lock_this(mGuardMutex, std::defer_lock);
	std::unique_lock<std::mutex> lock_ref(ref.mGuardMutex, std::defer_lock);
	std::lock(lock_this, lock_ref);

	mLockLogFile.set(std::move(ref.mLockLogFile));
	mLockName = std::move(ref.mLockName);
	mTargetOID = ref.mTargetOID;

	for (int i = 1; i < TRACK_LOCKS_MAX_HISTORY; ++i) {
		mLockHolders[i] = std::move(ref.mLockHolders[i]);
	}

	return *this;
}

#else // not TRACK_LOCKS

void Lockable::logLockError(const char* modifier, const String& msg, Thread* thread, const char* file, const char* function, int line, bool fatal) const {
	Time now;
	auto threadName = thread != nullptr ? thread->getName() : "<nullptr thread>";

	System::out
		<< now.getFormattedTime() << " " << threadName << " " << getLockName()
		<< msg
		<< " from " << file << ":" << line << " " << function << "()\n";

	if (fatal) {
		StackTrace::printStackTrace();

		assert(0 && "fatal lock error");
	}
}

#endif // TRACK_LOCKS
