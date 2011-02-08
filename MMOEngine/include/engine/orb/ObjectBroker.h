/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECTBROKER_H_
#define OBJECTBROKER_H_

#include "system/lang.h"

#include "object/DistributedObject.h"
#include "object/DistributedObjectStub.h"
#include "object/DistributedObjectClassHelper.h"

#include "object/ObjectNotDeployedException.h"
#include "object/ObjectNotLocalException.h"

#include "packets/DeployObjectMessage.h"
#include "packets/MethodReturnMessage.h"

namespace engine {
  namespace ORB {

	class ObjectBroker {
	public:
		virtual ~ObjectBroker() {
		}

		virtual void registerClass(const String& name, DistributedObjectClassHelper* helper) = 0;

		// deployment methods
		virtual void deploy(DistributedObjectStub* obj) = 0;
		virtual void deploy(const String& name, DistributedObjectStub* obj) = 0;

		virtual DistributedObjectStub* undeploy(const String& name) = 0;

		virtual DistributedObject* lookUp(const String& name) = 0;
		virtual DistributedObject* lookUp(uint64 objid) = 0;

		virtual bool destroyObject(DistributedObjectStub* obj) = 0;
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*OBJECTBROKER_H_*/
