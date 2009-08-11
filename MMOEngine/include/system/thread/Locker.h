/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOCKER_H_
#define LOCKER_H_

#include "Lockable.h"

namespace sys {
  namespace thread {

	class Locker  {
		Lockable* lockable;

		bool doLock;

	public:
		Locker(Lockable* lockable, bool doLock = true) {
			Locker::lockable = lockable;
			Locker::doLock = doLock;

			lockable->lock(doLock);
		}

		virtual ~Locker() {
			lockable->unlock(doLock);
		}
	};

  } // namespace thread
} //namespace sys

using namespace sys::thread;

#endif /*LOCKER_H_*/
