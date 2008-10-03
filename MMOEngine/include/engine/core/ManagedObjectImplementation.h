/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MANAGEDOBJECTIMPLEMENTATION_H_
#define MANAGEDOBJECTIMPLEMENTATION_H_

#include "ManagedObject.h"

namespace engine {
  namespace core {

	class ManagedObjectImplementation : public ManagedObjectServant {
	public:
		void lock(bool doLock = true);
		void lock(ManagedObject* obj);

		void wlock(bool doLock = true);
		void wlock(ManagedObject* obj);

		void unlock(bool doLock = true);

		void setLockName(const string& name);
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*MANAGEDOBJECTIMPLEMENTATION_H_*/
