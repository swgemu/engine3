/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOCKABLE_H_
#define LOCKABLE_H_

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)   // no-op
#endif

//#define THREAD_ANNOTATION_ATTRIBUTE__(x)   __attribute__((x))

#define CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY \
  THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define ACQUIRED_BEFORE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define REQUIRES_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RELEASE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RELEASE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define EXCLUDES(...) \
  THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RETURN_CAPABILITY(x) \
  THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS \
  THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

#include "../platform.h"

#include <pthread.h>

#include "../lang/String.h"

#include "../lang/StackTrace.h"
#include "../lang/Time.h"

#include "atomic/AtomicInteger.h"

#include "Thread.h"

namespace sys {
  namespace lang {
	  class Time;
  }
}

namespace sys {
  namespace thread {

	class Mutex;

	class CAPABILITY("mutex") Lockable {
	protected:
		//String lockName;

		Thread* threadLockHolder;
		AtomicInteger readLockCount;

#ifdef TRACE_LOCKS
		StackTrace* trace;
		StackTrace* unlockTrace;

		Time lockTime;

		bool doTrace;
#endif

#ifdef LOG_LOCKS
		AtomicInteger lockCount;
		int currentCount;

		bool doLog;
#endif

	public:
		Lockable();
		Lockable(const String& s);

		virtual ~Lockable();

		ACQUIRE() virtual void lock(bool doLock = true) = 0;
		ACQUIRE() virtual void lock(Lockable* lockable) = 0;

		RELEASE() virtual void unlock(bool doLock = true) = 0;

	protected:
		inline void lockAcquiring(const char* modifier = "") {
		#ifdef LOG_LOCKS
			int cnt = lockCount.increment();

			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName
							<< "] acquiring " << modifier << "lock #" << cnt << "\n";
		#endif
		}

		inline void lockAcquiring(Lockable* lockable, const char* modifier = "") {
		#ifdef LOG_LOCKS
			int cnt = lockCount.increment();

			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName
							<< " (" << lockable->lockName << ")] acquiring cross "
							<< modifier << "lock #" << cnt << "\n";
		#endif
		}

		inline void lockAcquired(const char* modifier = "") {
		#ifdef LOG_LOCKS
			currentCount = cnt;

			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName
							<< "] acquired " << modifier << "lock #" << cnt << "\n";
		#endif

		#ifdef TRACE_LOCKS
			if (modifier[0] != 'r')
				refreshTrace();
		#endif

			threadLockHolder = Thread::getCurrentThread();
		}

		inline void lockAcquired(Lockable* lockable, const char* modifier = "") {
		#ifdef LOG_LOCKS
			currentCount = cnt;

			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName
							<< " (" << lockable->lockName << ")] acquired cross "
							<< modifier << "lock #" << cnt << "\n";
		#endif

		#ifdef TRACE_LOCKS
			if (modifier[0] != 'r')
				refreshTrace();
		#endif

			threadLockHolder = Thread::getCurrentThread();
		}

		inline void lockReleasing(const char* modifier = "") {
			threadLockHolder = NULL;

		#ifdef TRACE_LOCKS
			if (modifier[0] != 'r') {
				deleteTrace();

				refreshUnlockTrace();
			}
		#endif

		#ifdef LOG_LOCKS
			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName
							<< "] releasing " << modifier << "lock #" << currentCount << "\n";
		#endif
		}

		inline void lockReleased(const char* modifier = "") {
		#ifdef LOG_LOCKS
			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName
							<< "] released " << modifier << "lock #" << currentCount << "\n";
		#endif
		}

		void traceDeadlock(const char* modifier = "");

		inline void refreshTrace() {
		#ifdef TRACE_LOCKS
			if (doTrace) {
				if (trace != NULL)
					delete trace;

				trace = new StackTrace();
			}
		#endif

		}

		inline void deleteTrace() {
		#ifdef TRACE_LOCKS
			if (doTrace) {
				if (trace != NULL) {
					delete trace;
					trace = NULL;
				}
			}
		#endif
		}

		inline void refreshUnlockTrace() {
		#ifdef TRACE_LOCKS
			if (doTrace) {
				if (unlockTrace != NULL)
					delete unlockTrace;

				unlockTrace = new StackTrace();
			}
		#endif
		}

		inline void deleteUnlockTrace() {
		#ifdef TRACE_LOCKS
			if (doTrace) {
				if (unlockTrace != NULL) {
					delete unlockTrace;
					unlockTrace = NULL;
				}
			}
		#endif
		}

	public:
		inline bool isLockedByCurrentThread() const {
			return threadLockHolder == Thread::getCurrentThread();
		}

		inline bool isReadLocked() const {
			 return readLockCount > 0;
		}

		inline Thread* getLockHolderThread() {
			return threadLockHolder;
		}

		// setters
		inline void setLockName(const String& s) {
			//lockName = s;
		}

		inline void setLockLogging(bool dolog) {
#ifdef LOG_LOCKS
			doLog = dolog;
#endif
		}

		inline void setLockTracing(bool tracing) {
#ifdef TRACE_LOCKS
			doTrace = tracing;
#endif
		}
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;



#endif /*LOCKABLE_H_*/
