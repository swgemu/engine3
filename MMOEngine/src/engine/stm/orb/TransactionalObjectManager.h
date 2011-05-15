/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TRANSACTIONALOBJECTMANAGER_H_
#define TRANSACTIONALOBJECTMANAGER_H_

#include "system/lang.h"

#include "engine/orb/ObjectBroker.h"
#include "engine/orb/DistributedObjectBroker.h"

#include "LocalObjectManager.h"

#include "engine/util/Command.h"

namespace engine {
  namespace stm {

  	class TransactionalObjectManager : public ObjectBroker, public Command {
  		DistributedObjectBroker* objectBroker;

  		ThreadLocal<LocalObjectManager*> localObjectManager;

  	public:
  		TransactionalObjectManager();

  		void initialize();

  		void execute();

  		void undo();

		void registerClass(const String& name, DistributedObjectClassHelper* helper);

		// deployment methods
		void deploy(DistributedObjectStub* obj);
		void deploy(const String& name, DistributedObjectStub* obj);

		DistributedObjectStub* undeploy(const String& name);

		DistributedObject* lookUp(const String& name);
		DistributedObject* lookUp(uint64 objid);

		bool destroyObject(DistributedObjectStub* obj);

  	protected:
		LocalObjectManager* getLocalObjectManager();
  	};

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* TRANSACTIONALOBJECTMANAGER_H_ */
