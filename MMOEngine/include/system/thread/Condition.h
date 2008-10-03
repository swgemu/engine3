/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef CONDITION_H_
#define CONDITION_H_

#include "../platform.h"

#include <pthread.h>

#include "../lang/Time.h"

namespace sys {
  namespace thread {

	class Condition {
		pthread_mutex_t cmutex;
		pthread_cond_t cond;
	
	public:
		Condition() {
			//cmutex = PTHREAD_MUTEX_INITIALIZER;
			pthread_mutex_init(&cmutex, NULL);
				
			//cond = PTHREAD_COND_INITIALIZER;
			pthread_cond_init(&cond, NULL);
		}

		virtual ~Condition() {
			pthread_cond_destroy(&cond);
		}
		
		inline void init() {
			pthread_cond_init(&cond, NULL);
		}
	
		inline int wait(Mutex* m) {
			int res = pthread_cond_wait(&cond, &m->mutex);
			if (res != 0)
				cout << "outer wait() failed on Condition (" << res << ")\n";
					
			return res;
		}
	
		inline void wait() {
			pthread_mutex_lock(&cmutex);
	
			int res = pthread_cond_wait(&cond, &cmutex);
			if (res != 0)
				cout << "wait() failed on Condition (" << res << ")\n";
	
			pthread_mutex_unlock(&cmutex);
		}
	
		inline int timedWait(Mutex* m, Time* time) {
			int res = pthread_cond_timedwait(&cond, &(m->mutex), time->getTimeSpec());
			/*if (res != 0 && res != 116) {
				cout << "outer locked timedwait() failed on Condition (" << res << ")\n";
				//return false;
			} //else
				//return true;*/
					
			return res;
		}
	
		inline bool timedWait(Time* time) {
			pthread_mutex_lock(&cmutex);
			int res = pthread_cond_timedwait(&cond, &cmutex, time->getTimeSpec());
			pthread_mutex_unlock(&cmutex);
	
			if (res != 0 && res != 116) {
				cout << "timedwait() failed on Condition (" << res << ")\n";
				return false;
			} else
				return true;
		}
	
		inline void signal(Mutex* m) {
			int res = pthread_cond_signal(&cond);
			if (res != 0)
				cout << "outer signal() failed on Condition (" << res << ")\n";
		}
	
		inline void signal() {
			pthread_mutex_lock(&cmutex);
	
			int res = pthread_cond_signal(&cond);
			if (res != 0)
				cout << "signal() failed on Condition (" << res << ")\n";
	
			pthread_mutex_unlock(&cmutex);
		}
	
		inline void broadcast(Mutex* m) {
			int res = pthread_cond_broadcast(&cond);
			if (res != 0)
				cout << "outer broadcast() failed on Condition (" << res << ")\n";
		}
	
		inline void broadcast() {
			pthread_mutex_lock(&cmutex);
	
			int res = pthread_cond_broadcast(&cond);
			if (res != 0)
				cout << "broadcast() failed on Condition (" << res << ")\n";
	
			pthread_mutex_unlock(&cmutex);
		}
	
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*CONDITION_H_*/
