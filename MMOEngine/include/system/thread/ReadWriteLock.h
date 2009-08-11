/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef READWRITELOCK_H_
#define READWRITELOCK_H_

#include "Lockable.h"

namespace sys {
  namespace thread {

	class Mutex;

	class ReadWriteLock : public Lockable  {
	protected:
		pthread_rwlock_t rwlock;

		uint32 threadIDLockHolder;

	public:
		ReadWriteLock() : Lockable() {
			pthread_rwlock_init(&rwlock, NULL);

			threadIDLockHolder = 0;
		}

		ReadWriteLock(const String& s) : Lockable(s) {
			pthread_rwlock_init(&rwlock, NULL);

			threadIDLockHolder = 0;
		}

		virtual ~ReadWriteLock() {
			pthread_rwlock_destroy(&rwlock);

			if (trace != NULL) {
				delete trace;
				trace = NULL;
			}

			if (unlockTrace != NULL) {
				delete unlockTrace;
				unlockTrace = NULL;
			}
		}

		inline void lock(bool doLock = true) {
			wlock(doLock);
		}

		inline void rlock(bool doLock = true) {
			if (!doLock)
				return;

			lockAcquiring("r");

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
					if (trace != NULL)
						trace->print();

					while (true) ;

					start.addMiliTime(1000);
		    	}

				lockTime.updateToCurrentTime();

				/*if (trace != NULL)
					delete trace;

				trace = new StackTrace();*/
			#endif

			lockAcquired("r");
		}

		inline void wlock(bool doLock = true) {
			if (!doLock)
				return;

			lockAcquiring("w");

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

					while (true) ;

					start.addMiliTime(1000);
		    	}

				lockTime.updateToCurrentTime();

				refreshTrace();

				threadIDLockHolder = Thread::getCurrentThreadID();
			#endif

			lockAcquired("w");
		}

		void wlock(Mutex* lock);

		void wlock(ReadWriteLock* lock);

		void lock(Lockable* lockable);

		inline bool tryWLock() {
			return pthread_rwlock_trywrlock(&rwlock) == 0;
		}

		inline void unlock(bool doLock = true) {
			if (!doLock)
				return;

			#if defined(TRACE_LOCKS) && !defined(__CYGWIN__)
				if (threadIDLockHolder == 0) {
					System::out << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "]"
							<< " unlocking an unlocked mutex\n";
					StackTrace::printStackTrace();

					if (unlockTrace != NULL) {
						System::out << "previously unlocked by\n";
						unlockTrace->print();
					}

					raise(SIGSEGV);
				} else if (threadIDLockHolder != Thread::getCurrentThreadID()) {
					System::out << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "]" << " mutex unlocked by a different thread\n";
					StackTrace::printStackTrace();

					if (trace != NULL) {
						System::out << "previously locked at " << lockTime.getMiliTime() << " by\n";
						trace->print();
					}
				}

				deleteTrace();

				threadIDLockHolder = 0;

				refreshUnlockTrace();
			#endif

			lockReleasing();

			int res = pthread_rwlock_unlock(&rwlock);
			if (res != 0) {
				System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on RWLock \'" << lockName << "\' (" << res << ")\n";

				StackTrace::printStackTrace();
			}

			lockReleased();
		}

		inline void runlock(bool doLock = true) {
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

			/*delete trace;
			trace = NULL;

			threadIDLockHolder = 0;*/
		#endif

			lockReleasing("r");

			int res = pthread_rwlock_unlock(&rwlock);
			if (res != 0) {
				System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on RWLock \'" << lockName << "\' (" << res << ")\n";

				StackTrace::printStackTrace();
			}

			lockReleased();
		}

		inline bool destroy() {
			pthread_rwlock_wrlock(&rwlock);
			pthread_rwlock_unlock(&rwlock);

			return pthread_rwlock_destroy(&rwlock) == 0;
		}

		friend class Condition;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*READWRITELOCK_H_*/
