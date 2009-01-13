/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef READWRITELOCK_H_
#define READWRITELOCK_H_

#include "../platform.h"

#include <pthread.h>

#include "../lang/StackTrace.h"
#include "Atomic.h"
#include "Thread.h"

namespace sys {
  namespace thread {

	class Mutex;

	class ReadWriteLock {
	protected:
		pthread_rwlock_t rwlock;

		String lockName;
		int lockCount;

		StackTrace* trace;
		Time lockTime;

		bool doTrace;

		uint32 threadIDLockHolder;

	public:
		ReadWriteLock() {
			pthread_rwlock_init(&rwlock, NULL);

			lockCount = 0;

			trace = NULL;
			doTrace = true;

			threadIDLockHolder = 0;
		}

		ReadWriteLock(const String& s) {
			pthread_rwlock_init(&rwlock, NULL);

			lockName = s;
			lockCount = 0;

			trace = NULL;
			doTrace = true;

			threadIDLockHolder = 0;
		}

		virtual ~ReadWriteLock() {
			pthread_rwlock_destroy(&rwlock);

			if (trace != NULL) {
				delete trace;
				trace = NULL;
			}
		}

		inline void rlock(bool doLock = true) {
			if (!doLock)
				return;

			#ifdef LOG_LOCKS
				Atomic::incrementInt((uint32*)&lockCount);
				int cnt = lockCount;

				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquiring rlock #" << cnt << "\n";
			#endif

			#if !defined(TRACE_LOCKS) || defined(__CYGWIN__)
				int res = pthread_rwlock_rdlock(&rwlock);
				if (res != 0)
					System::out << "(" << Time::currentNanoTime() << " nsec) rlock() failed on RWLock \'" << lockName << "\' (" << res << ")\n";
			#else
				#ifndef LOG_LOCKS
					Atomic::incrementInt((uint32*)&lockCount);
					int cnt = lockCount;
				#endif


				Time start;
				start.addMiliTime(300000);

		    	while (pthread_rwlock_timedrdlock(&rwlock, start.getTimeSpec())) {
		    		if (!doTrace)
		    			continue;

		    		System::out << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "] unable to access rlock #" << cnt << " at\n";
					StackTrace::printStackTrace();

					System::out << "locked at " << lockTime.getMiliTime() << " by\n";
					trace->print();

					while (true);

					start.addMiliTime(1000);
		    	}

				lockTime.update();

				if (trace != NULL)
					delete trace;

				trace = new StackTrace();
			#endif

			#ifdef LOG_LOCKS
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquired rlock #" << cnt << "\n";
			#endif
		}

		inline void wlock(bool doLock = true) {
			if (!doLock)
				return;

			#ifdef LOG_LOCKS
				Atomic::incrementInt((uint32*)&lockCount);
				int cnt = lockCount;

				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquiring wlock #" << cnt << "\n";
			#endif

			#if !defined(TRACE_LOCKS) || defined(__CYGWIN__)
				int res = pthread_rwlock_wrlock(&rwlock);
				if (res != 0)
					System::out << "(" << Time::currentNanoTime() << " nsec) wlock() failed on RWLock \'" << lockName << "\' (" << res << ")\n";
			#else
				#ifndef LOG_LOCKS
					Atomic::incrementInt((uint32*)&lockCount);
					int cnt = lockCount;
				#endif

				Time start;
				start.addMiliTime(300000);

		    	while (pthread_rwlock_timedwrlock(&rwlock, start.getTimeSpec())) {
		    		if (!doTrace)
		    			continue;

		    		System::out << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "] unable to access wlock #" << cnt << " at\n";
		    		StackTrace::printStackTrace();

					if (trace != NULL) {
						System::out << "locked at " << lockTime.getMiliTime() << " by\n";
						trace->print();
					} else {
						System::out << "no previous stackTrace created\n";
					}

					while (true);

					start.addMiliTime(1000);
		    	}

				lockTime.update();

				if (trace != NULL)
					delete trace;

				trace = new StackTrace();

				threadIDLockHolder = Thread::getCurrentThreadID();
			#endif

			#ifdef LOG_LOCKS
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquired wlock #" << cnt << "\n";
			#endif
		}

		void wlock(Mutex* lock);

		void wlock(ReadWriteLock* lock);

		inline bool tryWLock() {
			return pthread_rwlock_trywrlock(&rwlock) == 0;
		}

		inline void unlock(bool doLock = true) {
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

					if (trace != NULL) {
						System::out << "previously locked at " << lockTime.getMiliTime() << " by\n";
						trace->print();
					}
				}*/

				delete trace;
				trace = NULL;

				threadIDLockHolder = 0;
			#endif

			#ifdef LOG_LOCKS
				int cnt = lockCount;
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] releasing lock #" << cnt << "\n";
			#endif

			int res = pthread_rwlock_unlock(&rwlock);
			if (res != 0) {
				System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on RWLock \'" << lockName << "\' (" << res << ")\n";

				StackTrace::printStackTrace();
			}

			#ifdef LOG_LOCKS
				System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] released lock #" << cnt << "\n";
			#endif
		}

		inline bool destroy() {
			pthread_rwlock_wrlock(&rwlock);
			pthread_rwlock_unlock(&rwlock);

			return pthread_rwlock_destroy(&rwlock) == 0;
		}

		// setters
		inline void setLockName(const String& s) {
			lockName = s;
		}

		inline void setLockTracing(bool tracing) {
			doTrace = tracing;
		}

		friend class Condition;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*READWRITELOCK_H_*/
