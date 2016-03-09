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

		bool doLock;

	public:
		ReadLocker(ReadWriteLock* lock) ACQUIRE_SHARED(lock) {
			ReadLocker::doLock = !lock->isLockedByCurrentThread();

			ReadLocker::lockable = lock;
			lock->rlock(doLock);
		}

		virtual ~ReadLocker() RELEASE() {
			if (lockable != NULL) {
				lockable->runlock(doLock);
				lockable = NULL;
			}
		}

	public:
		inline void release() RELEASE() {
			if (lockable != NULL) {
				lockable->runlock(doLock);
				lockable = NULL;
			}
		}
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /* READLOCKER_H_ */
