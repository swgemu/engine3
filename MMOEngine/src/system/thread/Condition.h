/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef CONDITION_H_
#define CONDITION_H_

#include "system/platform.h"

#include <pthread.h>

#include <sys/time.h>

#include <errno.h>

#include "system/lang/Time.h"
#include "system/thread/Mutex.h"

namespace sys {
  namespace thread {

	class Condition {
		pthread_mutex_t cmutex;
		pthread_cond_t cond;

		int signalCount;
		int waiterCount;

	public:
		Condition() {
			//cmutex = PTHREAD_MUTEX_INITIALIZER;
			pthread_mutex_init(&cmutex, nullptr);

			//cond = PTHREAD_COND_INITIALIZER;
			pthread_cond_init(&cond, nullptr);

			signalCount = 0;
			waiterCount = 0;
		}

		virtual ~Condition() {
			pthread_cond_destroy(&cond);
			pthread_mutex_destroy(&cmutex);
		}

		inline void init() {
			pthread_cond_init(&cond, nullptr);
		}

		inline int wait(Mutex* m) {
			return doWait(&(m->mutex));
		}

		inline int wait() {
			pthread_mutex_lock(&cmutex);

			int res = doWait(&cmutex);

			pthread_mutex_unlock(&cmutex);
			return res;
		}

		inline int timedWait(Mutex* m, const Time* time) {
			return doTimedWait(&(m->mutex), time);
		}

		inline int timedWait(const Time* time) {
			pthread_mutex_lock(&cmutex);

			int res = doTimedWait(&cmutex, time);

			pthread_mutex_unlock(&cmutex);
			return res;
		}

		inline void signal(Mutex* m) {
			doSignal();
		}

		inline void signal() {
			pthread_mutex_lock(&cmutex);

			doSignal();

			pthread_mutex_unlock(&cmutex);
		}

		inline void broadcast(Mutex* m) {
			doBroadcast();
		}

		inline void broadcast() {
			pthread_mutex_lock(&cmutex);

			doBroadcast();

			pthread_mutex_unlock(&cmutex);
		}

	private:
		inline int doWait(pthread_mutex_t* mutex) {
			if (signalCount > 0) {
				--signalCount;

				return 0;
			}

			waiterCount++;

			int res = pthread_cond_wait(&cond, mutex);
			if (res != 0)
				System::out << "outer wait() failed on Condition (" << res << ")\n";

			waiterCount--;

			return res;
		}

		inline int doTimedWait(pthread_mutex_t* mutex, const Time* time) {
			if (signalCount > 0) {
				--signalCount;

				/*struct timespec trem;
				clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, time->getTimeSpec(), &trem);*/

				return 0;
			}

			waiterCount++;

			int res = pthread_cond_timedwait(&cond, mutex, time->getTimeSpec());

			waiterCount--;

			if (res != 0 && res != ETIMEDOUT) {
				System::out << "timedwait() failed on Condition (" << res << ")\n";
				return res;
			}

			return res;
		}

		inline void doSignal() {
			if (waiterCount == 0) {
				signalCount++;
				return;
			}

			int res = pthread_cond_signal(&cond);
			if (res != 0)
				System::out << "outer signal() failed on Condition (" << res << ")\n";
		}

		inline void doBroadcast() {
			int res = pthread_cond_broadcast(&cond);
			if (res != 0)
				System::out << "outer broadcast() failed on Condition (" << res << ")\n";
		}
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*CONDITION_H_*/
