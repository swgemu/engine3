/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MUTEX_H_
#define MUTEX_H_

#include "Lockable.h"

namespace sys {
  namespace thread {

	class ReadWriteLock;

	class Mutex : public Lockable {
		pthread_mutex_t mutex;

	public:
		Mutex() : Lockable() {
			pthread_mutex_init(&mutex, NULL);
		}

		Mutex(const String& s) : Lockable(s) {
			pthread_mutex_init(&mutex, NULL);
		}

		virtual ~Mutex() {
			pthread_mutex_destroy(&mutex);
		}

		inline void lock(bool doLock = true) {
			if (!doLock)
				return;

			lockAcquiring();

			#if !defined(TRACE_LOCKS) || defined(PLATFORM_CYGWIN)
				int res = pthread_mutex_lock(&mutex);

				if (res != 0)
					System::out << "(" << Time::currentNanoTime() << " nsec) lock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";
			#else
				Time start;
				start.addMiliTime(300000);

		    	while (pthread_mutex_timedlock(&mutex, start.getTimeSpec())) {
		    		if (!doTrace)
		    			continue;

		    		traceDeadlock();

					start.addMiliTime(1000);
		    	}

				lockTime.updateToCurrentTime();
			#endif

			lockAcquired();
		}

		inline void lock(Mutex* m) {
			if (this == m) {
				System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross locking itself [" << lockName << "]\n";

				StackTrace::printStackTrace();
				return;
			}

			lockAcquiring(m);

		    while (pthread_mutex_trylock(&mutex)) {
		    	#ifndef TRACE_LOCKS
		   			pthread_mutex_unlock(&(m->mutex));
			       	pthread_mutex_lock(&(m->mutex));
			  	#else
			  		m->unlock();
			  		m->lock();
			  	#endif
	   		}

			lockAcquired(m);
		}

		inline void lock(Lockable* lockable) {
			if (this == lockable) {
				System::out << "(" << Time::currentNanoTime() << " nsec) ERROR: cross locking itself [" << lockName << "]\n";

				StackTrace::printStackTrace();
				return;
			}

			lockAcquiring(lockable);

		    while (pthread_mutex_trylock(&mutex)) {
				lockable->unlock();
				lockable->lock();
	   		}

			lockAcquired(lockable);
		}

		inline bool tryLock() {
			return pthread_mutex_trylock(&mutex) == 0;
		}

		inline void unlock(bool doLock = true) {
			if (!doLock)
				return;

			lockReleasing();

			int res = pthread_mutex_unlock(&mutex);
			if (res != 0) {
				System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";

				StackTrace::printStackTrace();
			}

			lockReleased();
		}

		friend class Condition;
		friend class ReadWriteLock;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*MUTEX_H_*/
