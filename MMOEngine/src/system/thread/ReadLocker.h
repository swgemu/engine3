/*
 * ReadReadLocker.h
 *
 *  Created on: 20/06/2012
 *      Author: victor
 */

#ifndef READLOCKER_H_
#define READLOCKER_H_

#include "ReadWriteLock.h"

namespace sys {
  namespace thread {

	class SCOPED_CAPABILITY ReadLocker  {
		ReadWriteLock* lockable;
	public:
		ReadLocker(ReadLocker&& locker) : lockable(locker.lockable) {
			locker.lockable = nullptr;
		}

		ReadLocker(ReadWriteLock* lock) ACQUIRE_SHARED(lock) {
			const auto doLock = !lock->isLockedByCurrentThread();

			if (doLock) {
				lockable = lock;

				lock->rlock();
			} else {
				lockable = nullptr;
			}
		}

	       	ReadLocker(const ReadLocker&) = delete;
		ReadLocker& operator=(const ReadLocker&) = delete;

		~ReadLocker() RELEASE() {
			if (lockable != nullptr) {
				lockable->runlock();
			}
		}

	public:
		inline void release() RELEASE() {
			if (lockable != nullptr) {
				lockable->runlock();
			}
		}
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /* READLOCKER_H_ */
