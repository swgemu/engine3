/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECTSERVANT_H_
#define DISTRIBUTEDOBJECTSERVANT_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {
  	
  	class DistributedObjectStub;

  	class DistributedObjectClassHelper;

	class DistributedObjectServant {
	protected:
		DistributedObjectClassHelper* _classHelper;
		
		DistributedObjectStub* _stub;
		
	public:
		DistributedObjectServant();
		
		virtual ~DistributedObjectServant();

		/*void lock(bool doLock = true);

		void rlock(bool doLock = true);

		void wlock(bool doLock = true);

		void wlock(Mutex* lock);

		void wlock(ReadWriteLock* lock);

		void unlock(bool doLock = true);

		void runlock(bool doLock = true);*/

		void setDeployingName(const String& name);

		virtual void _setStub(DistributedObjectStub* stub) = 0;

		virtual DistributedObjectStub* _getStub() = 0;
		
		// setters
		inline void _setClassHelper(DistributedObjectClassHelper* helper) {
			_classHelper = helper;
		}

		// getters
		inline DistributedObjectClassHelper* _getClassHelper() {
			return _classHelper;
		}	
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTSERVANT_H_*/
