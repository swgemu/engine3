/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECTBROKER_H_
#define OBJECTBROKER_H_

#include "system/lang.h"

#include "NameAlreadyBoundException.h"

#include "object/ObjectAlreadyDeployedException.h"
#include "object/ObjectNotDeployedException.h"
#include "object/ObjectNotLocalException.h"

namespace engine {
  namespace ORB {

  	class DistributedObjectClassHelper;

  	class DistributedObject;
  	class DistributedObjectStub;
  	class DOBObjectManager;

	class ObjectBroker {
	public:
		virtual ~ObjectBroker() {
		}

		virtual void registerClass(const String& name, DistributedObjectClassHelper* helper) = 0;

		// deployment methods
		virtual void deploy(DistributedObjectStub* obj) = 0;
		virtual void deploy(const String& name, DistributedObjectStub* obj) = 0;

		virtual void requestServant(DistributedObjectStub* obj) = 0;

		virtual DistributedObjectStub* undeploy(const String& name) = 0;

		virtual Reference<DistributedObject*> lookUp(const String& name) = 0;
		virtual Reference<DistributedObject*> lookUp(uint64 objid) = 0;

		virtual bool destroyObject(DistributedObjectStub* obj) = 0;
		
		virtual DOBObjectManager* getObjectManager() = 0;

		virtual uint64 getNextFreeObjectID() = 0;

		int compareTo(ObjectBroker*& b) {
			if (this < b)
				return 1;
			else if (this > b)
				return -1;
			else
				return 0;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*OBJECTBROKER_H_*/
