/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef LOCALOBJECTMANAGER_H_
#define LOCALOBJECTMANAGER_H_

#include "system/lang.h"

#include "engine/orb/ObjectBroker.h"
#include "engine/orb/DistributedObjectBroker.h"

#include "engine/log/Logger.h"

namespace engine {
  namespace stm {

  	class LocalObjectManager : public ObjectBroker, public Logger {
  		DistributedObjectBroker* objectBroker;
  		DOBObjectManager* objectManager;

  		HashTable<uint64, Reference<DistributedObjectStub*> > localObjectDirectory;

  		HashTable<String, Reference<DistributedObjectStub*> > localNamingDirectory;

  		SortedVector<Reference<DistributedObjectStub*> > undeployedObjects;
  		SortedVector<Reference<DistributedObjectStub*> > destroyedObjects;

  	public:
  		LocalObjectManager();

  		void commitObjectChanges();

  		void clearObjectChanges();

		void registerClass(const String& name, DistributedObjectClassHelper* helper);

		void requestServant(DistributedObjectStub* obj) {

		}

		// deployment methods
		void deploy(DistributedObjectStub* obj);
		void deploy(const String& name, DistributedObjectStub* obj);

		DistributedObjectStub* undeploy(const String& name);

		Reference<DistributedObject*> lookUp(const String& name);
		Reference<DistributedObject*> lookUp(uint64 objid);
		
		DOBObjectManager* getObjectManager() {
                    return objectManager;
                }

		bool destroyObject(DistributedObjectStub* obj);

		uint64 getNextFreeObjectID();
  	};

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* LOCALOBJECTMANAGER_H_ */
