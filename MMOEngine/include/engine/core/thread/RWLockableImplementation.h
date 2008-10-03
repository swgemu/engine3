/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef RWLOCKABLEIMPLEMENTATION_H_
#define RWLOCKABLEIMPLEMENTATION_H_

#include "../../../system/lang.h"

#include "RWLockable.h" 

namespace engine {
  namespace core {
  	namespace thread {

	class RWLockableImplementation : public ReadWriteLock, public RWLockableServant {
	public:
		void wlock(bool doLock = true) {
			ReadWriteLock::wlock(doLock);
		}
		
		void wlock(RWLockable* lock) {
			if (_this == lock) {
				cout << "ERROR: cross wlocking itself [" << lockName << "]\n";
					
				StackTrace::printStackTrace();
				return;
			}
				
		    while (!tryWLock()) {
	       		lock->unlock();
	       		lock->wlock();
	   		}
		}
		
	};

    } // namespace thread
  } // namespace core
} // namespace engine

using namespace engine::core::thread;

#endif /*RWLOCKABLEIMPLEMENTATION_H_*/
