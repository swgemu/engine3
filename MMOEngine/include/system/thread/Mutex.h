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

		bool locked;

	public:
		Mutex() : Lockable() {
			pthread_mutex_init(&mutex, NULL);

			locked = false;
		}

		Mutex(const String& s) : Lockable(s) {
			pthread_mutex_init(&mutex, NULL);

			locked = false;
		}

		~Mutex() {
			pthread_mutex_destroy(&mutex);
		}

		inline void lock(bool doLock = true) {
			if (!doLock)
				return;

			lockAcquiring();

			#if !defined(TRACE_LOCKS) || defined(__CYGWIN__)
				int res = pthread_mutex_lock(&mutex);

				if (res != 0)
					System::out << "(" << Time::currentNanoTime() << " nsec) lock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";
			#else
				#ifndef LOG_LOCKS
					//Atomic::incrementInt((uint32*)&lockCount);
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

				lockTime.updateToCurrentTime();

				refreshTrace();
			#endif

			locked = true;

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

		    refreshTrace();

			locked = true;

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

		    refreshTrace();

			locked = true;

			lockAcquired(lockable);
		}

		inline bool tryLock() {
			return pthread_mutex_trylock(&mutex) == 0;
		}

		inline void unlock(bool doLock = true) {
			if (!doLock)
				return;

			lockReleasing();

			deleteTrace();

			locked = false;

			int res = pthread_mutex_unlock(&mutex);
			if (res != 0) {
				System::out << "(" << Time::currentNanoTime() << " nsec) unlock() failed on Mutex \'" << lockName << "\' (" << res << ")\n";

				StackTrace::printStackTrace();
			}

			lockReleased();
		}

		// setters
		inline bool isLocked() {
			return locked;
		}

		friend class Condition;
		friend class ReadWriteLock;
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*MUTEX_H_*/
