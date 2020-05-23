/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef LOCKABLE_H_
#define LOCKABLE_H_

#include "system/platform.h"

#include <pthread.h>

#include "system/lang/String.h"

#include "system/lang/StackTrace.h"
#include "system/lang/Time.h"

#include "atomic/AtomicInteger.h"
#include "atomic/AtomicReference.h"

#include "Thread.h"

#define TRACE_BLOCK_TIME

#define TRACK_LOCKS

#if defined(TRACK_LOCKS) && defined(PLATFORM_CYGWIN)
#undef TRACK_LOCKS // Not supported on cygwin build
#endif

#ifdef TRACK_LOCKS
// NOTE: This eats ram fast: TRACK_LOCKS_MAX_HISTORY * Number_Of_Objects_in_Ram * sizeof(LockHolder) (24 Bytes) + Lockable data (58)
// Must be >= 2 to have room for one locker and one reader
#define TRACK_LOCKS_MAX_HISTORY 5

// First attempt timeout in ms, others are expoential backoff
#define TRACK_LOCKS_INITIAL_TIMEOUT 10000

// Second attempt timeout in ms, will delay exponentially from here
#define TRACK_LOCKS_RETRY_BASE 1000

// Amount of randomness to add to retry
#define TRACK_LOCKS_RETRY_JITTER 500

// Max amount of time backoff timeout will wait
#define TRACK_LOCKS_MAX_TIMEOUT 60000

// Report double unlocks (noisy and slow)
// #define TRACK_LOCKS_REPORT_DOUBLE_UNLOCK

// Report cross thread unlocks
// #define TRACK_LOCKS_REPORT_CROSS_UNLOCK

#include "LockHolder.h"
#include <mutex>
#endif

#ifdef ENABLE_YIELD_BETWEEN_CROSSLOCK
#define CROSSLOCK_YIELD() Thread::yield()
#else
#define CROSSLOCK_YIELD()
#endif

namespace sys {
  namespace lang {
	  class Time;
  }

  namespace io {
	  class FileWriter;
	  class File;
  }
}

namespace sys {
  namespace thread {

	class Mutex;

	class LockTracer;

	class CAPABILITY("mutex") Lockable {
	protected:
		AtomicReference<Thread*> threadLockHolder;

#ifdef TRACK_LOCKS

        mutable std::mutex mGuardMutex;
        AtomicReference<FileWriter*> mLockLogFile;
        char* mLockName = nullptr;
        uint64 mTargetOID = 0;
        LockHolder mLockHolders[TRACK_LOCKS_MAX_HISTORY];

		void logLockMessage(const String& msg) const;
		void logLockAction(const char* method, const char* modifier, const char* file, const char* func, int line) const;
		void writeLockLog(const String& msg, bool writeRaw = false) const;
		void traceDeadlock(const char* modifier, const char* file, const char* function, int line);
		inline int16 safeGetThreadNumber(Thread* thread) const {
			return (int16)(thread != nullptr ? thread->getThreadNumber() : 0);
		}
		Lockable& operator=(const Lockable& ref);

	public:
		void setLockLogging(bool dolog);
		void setTargetOID(uint64 oid);
		void setLockName(const String& name);
		const String getLockName() const;
		const String getLockHolderDetails() const;
		inline void setLockTracing(bool doTrace) {
			setLockLogging(doTrace);
		}

#else // TRACK_LOCKS

	public:
		inline void setLockLogging(bool dolog) {
		}

		inline void setLockTracing(bool dolog) {
		}

		inline void setTargetOID(uint64 oid) {
		}

		inline void setLockName(const String& name) {
		}

		inline const String getLockName() const {
			StringBuffer buf;

			buf << "[Lockable " << this << "]";

			return buf.toString();
		}

	protected:

#endif // TRACK_LOCKS

		void logLockResult(const char* modifier, Thread* thread, const char* file, const char* function, int line, int lockResult) const;

		void logLockResult(const char* modifier, const char* file, const char* function, int line, int lockResult) const {
			logLockResult(modifier, Thread::getCurrentThread(), file, function, line, lockResult);
		}

		void logLockError(const char* modifier, const String& msg, Thread* thread, const char* file, const char* function, int line, bool fatal = false) const;

		void logLockError(const char* modifier, const String& msg, const char* file, const char* function, int line, bool fatal = false) const {
			logLockError(modifier, msg, Thread::getCurrentThread(), file, function, line, fatal);
		}

	public:
		Lockable();

		Lockable(const String& name);

		virtual ~Lockable();

		ACQUIRE() virtual void lock(bool doLock = true, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) = 0;
		ACQUIRE() virtual void lock(Lockable* lockable, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) = 0;

		RELEASE() virtual void unlock(bool doLock = true, const char* file = __builtin_FILE(), const char* function = __builtin_FUNCTION(), int line = __builtin_LINE()) = 0;

	protected:
		uint64 lockAcquiring(const char* modifier, const char* file, const char* function, int line);

		uint64 lockAcquiring(Lockable* lockable, const char* modifier, const char* file, const char* function, int line);

		uint64 lockAcquired(const char* modifier, const char* file, const char* function, int line);

		uint64 lockAcquired(Lockable* lockable, const char* modifier, const char* file, const char* function, int line);

		void lockReleasing(const char* modifier, const char* file, const char* function, int line);

		void lockReleased(const char* modifier, const char* file, const char* function, int line);

		void clearCurrentLockHolder(const char* modifier, Thread* thread);

		void setCurrentLockHolder(const char* modifier, Thread*  thread, const char* file, const char* function, int line);

	public:
		inline bool isLockedByCurrentThread() const {
			return threadLockHolder.get() == Thread::getCurrentThread();
		}

		inline Thread* getLockHolderThread() {
			return threadLockHolder.get();
		}

		bool isReadLocked() const;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*LOCKABLE_H_*/
