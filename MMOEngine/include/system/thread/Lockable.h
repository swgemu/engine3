/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOCKABLE_H_
#define LOCKABLE_H_

#include "../platform.h"

#include <pthread.h>

#include "../lang/String.h"
#include "../lang/Time.h"

#include "../lang/StackTrace.h"

#include "Atomic.h"
#include "Thread.h"

namespace sys {
  namespace thread {

	class Mutex;

	class Lockable {
	protected:
		String lockName;

		int lockCount;
		int currentCount;

		StackTrace* trace;
		StackTrace* unlockTrace;

		Time lockTime;

		bool doLog;
		bool doTrace;

	public:
		Lockable() {
			lockCount = 0;
			currentCount = 0;

			trace = NULL;
			unlockTrace = NULL;

			doLog = true;
			doTrace = true;
		}

		Lockable(const String& s) {
			lockName = s;

			lockCount = 0;

			trace = NULL;
			unlockTrace = NULL;

			doLog = true;
			doTrace = true;
		}

		virtual ~Lockable() {
			if (trace != NULL) {
				delete trace;
				trace = NULL;
			}

			if (unlockTrace != NULL) {
				delete unlockTrace;
				unlockTrace = NULL;
			}
		}

		virtual void lock(bool doLock = true) = 0;
		virtual void lock(Lockable* lockable) = 0;

		virtual void unlock(bool doLock = true) = 0;

	protected:
		inline void lockAcquiring(const char* modifier = "") {
		#ifdef LOG_LOCKS
			Atomic::incrementInt(&lockCount);
			int cnt = lockCount;

			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquiring " << modifier << "lock #" << cnt << "\n";
		#endif
		}

		inline void lockAcquiring(Lockable* lockable, const char* modifier = "") {
		#ifdef LOG_LOCKS
			Atomic::incrementInt(&lockCount);
			int cnt = lockCount;

			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << " (" << lockable->lockName << ")] acquiring cross " << modifier << "lock #" << cnt << "\n";
		#endif
		}

		inline void lockAcquired(const char* modifier = "") {
		#ifdef LOG_LOCKS
			currentCount = cnt;

			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquired " << modifier << "lock #" << cnt << "\n";
		#endif
		}

		inline void lockAcquired(Lockable* lockable, const char* modifier = "") {
		#ifdef LOG_LOCKS
			currentCount = cnt;

			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << " (" << lockable->lockName << ")] acquired cross " << modifier << "lock #" << cnt << "\n";
		#endif
		}

		inline void lockReleasing(const char* modifier = "") {
		#ifdef LOG_LOCKS
			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] releasing " << modifier << "lock #" << currentCount << "\n";
		#endif
		}

		inline void lockReleased(const char* modifier = "") {
		#ifdef LOG_LOCKS
			if (doLog)
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] released " << modifier << "lock #" << currentCount << "\n";
		#endif
		}

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
		// setters
		inline void setLockName(const String& s) {
			lockName = s;
		}

		inline void setLockLogging(bool dolog) {
			doLog = dolog;
		}

		inline void setLockTracing(bool tracing) {
			doTrace = tracing;
		}
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*LOCKABLE_H_*/
