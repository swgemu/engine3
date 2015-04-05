/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOCKABLE_H_
#define LOCKABLE_H_

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

	class Lockable {
	protected:
		//String lockName;

		Thread* threadLockHolder;

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

		virtual void lock(bool doLock = true) = 0;
		virtual void lock(Lockable* lockable) = 0;

		virtual void unlock(bool doLock = true) = 0;

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
