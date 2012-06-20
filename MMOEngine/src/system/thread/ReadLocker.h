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

	class ReadLocker  {
		ReadWriteLock* lockable;

		bool doLock;

	public:
		ReadLocker(ReadWriteLock* lockable) {
			acquire(lockable);
		}

		virtual ~ReadLocker() {
			release();
		}

	protected:
		inline void acquire(ReadWriteLock* lockable) {
			ReadLocker::doLock = !lockable->isLockedByCurrentThread();

			ReadLocker::lockable = lockable;
			lockable->rlock(doLock);
		}

	public:
		inline void release() {
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
