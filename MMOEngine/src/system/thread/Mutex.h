/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MUTEX_H_
#define MUTEX_H_

#include "../platform.h"

#include <pthread.h>

#include "../lang/System.h"

#include "../lang/String.h"
#include "../lang/StackTrace.h"
#include "../lang/Time.h"

#include "Atomic.h"

namespace sys {
  namespace thread {

	class ReadWriteLock;

	class Mutex {
		pthread_mutex_t mutex;

		bool doLog;
		String lockName;

		int lockCount;
		int currentCount;

		StackTrace* trace;
		Time lockTime;

		bool doTrace;

	public:
		Mutex() {
			pthread_mutex_init(&mutex, NULL);

			doLog = true;
			lockName = "";

			lockCount = 0;

			trace = NULL;
			doTrace = true;
		}

		Mutex(const String& s) {
			pthread_mutex_init(&mutex, NULL);

			doLog = true;
			lockName = s;

			lockCount = 0;

			trace = NULL;
			doTrace = true;
		}

		virtual ~Mutex() {
			pthread_mutex_destroy(&mutex);

			if (trace != NULL) {
				delete trace;
				trace = NULL;
			}
		}

		inline void lock(bool doLock = true) {
			if (!doLock)
				return;

			#ifdef LOG_LOCKS
				Atomic::incrementInt(&lockCount);
				int cnt = lockCount;

				if (doLog)
					System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquiring lock #" << cnt << "\n";
			#endif

			#if !defined(TRACE_LOCKS) || defined(__CYGWIN__)
				int res = pthread_mutex_lock(&mutex);

				if (res != 0)
					System::out << "(" << Time::currentNanoTime() << " nsec) lock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";
			#else
				#ifndef LOG_LOCKS
					Atomic::incrementInt((uint32*)&lockCount);
					int cnt = lockCount;
				#endif

				Time start;
				start.addMiliTime(300000);

		    	while (pthread_mutex_timedlock(&mutex, start.getTimeSpec())) {
		    		if (!doTrace)
		    			continue;

	 	  			System::out << "(" << Time::currentNanoTime() << " nsec) WARNING" << "[" << lockName << "] unable to access lock #" << cnt << " at\n";
					StackTrace::printStackTrace();

					System::out << "locked at " << lockTime.getMiliTime() << " by\n";
					trace->print();

					while (true) ;

					start.addMiliTime(1000);
		    	}

				lockTime.update();

				if (trace != NULL)
					delete trace;

				trace = new StackTrace();
			#endif

			#ifdef LOG_LOCKS
				currentCount = cnt;

				if (doLog)
					System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] acquired lock #" << cnt << "\n";
			#endif
		}

		inline void lock(Mutex* m) {
			if (this == m) {
				System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross locking itself [" << lockName << "]\n";

				StackTrace::printStackTrace();
				return;
			}

			#ifdef LOG_LOCKS
				Atomic::incrementInt(&lockCount);
				int cnt = lockCount;

				if (doLog)
					System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << " (" << m->lockName << ")] acquiring cross lock #" << cnt << "\n";
			#endif

		    while (pthread_mutex_trylock(&mutex)) {
		    	#ifndef TRACE_LOCKS
		   			pthread_mutex_unlock(&(m->mutex));
			       	pthread_mutex_lock(&(m->mutex));
			  	#else
			  		m->unlock();
			  		m->lock();
			  	#endif
	   		}

		#ifdef TRACE_LOCKS
			if (doTrace) {
				delete trace;
				trace = new StackTrace();
			}
		#endif

			#ifdef LOG_LOCKS
				currentCount = cnt;

				if (doLog)
					System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << " (" << m->lockName << ")] acquired cross lock #" << cnt << "\n";
			#endif
		}

		inline bool tryLock() {
			return pthread_mutex_trylock(&mutex) == 0;
		}

		inline void unlock(bool doLock = true) {
			if (!doLock)
				return;

			#ifdef LOG_LOCKS
				if (doLog)
					System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] releasing lock #" << currentCount << "\n";
			#endif

			#ifdef TRACE_LOCKS
				delete trace;
				trace = NULL;
			#endif

			int res = pthread_mutex_unlock(&mutex);
			if (res != 0) {
				System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";

				StackTrace::printStackTrace();
			}

			#ifdef LOG_LOCKS
				if (doLog)
					System::out << "(" << Time::currentNanoTime() << " nsec) [" << lockName << "] released lock #" << currentCount << "\n";
			#endif
		}

		// setters
		inline void setMutexLogging(bool dolog) {
			doLog = dolog;
		}

		inline void setLockName(const String& s) {
			lockName = s;
		}

		inline void setLockTracing(bool tracing) {
			doTrace = tracing;
		}

		friend class Condition;
		friend class ReadWriteLock;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*MUTEX_H_*/
