/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

  class ObjectsToSaveMap : public HashTable<Reference<DistributedObject*>, Reference<DistributedObjectServant*> > {
	 /* int hash(const Reference<DistributedObject*>& key) {
		  return UnsignedLong::hashCode((uint64)key.get());
	  }*/

  public:
	  ObjectsToSaveMap() {

	  }

	  ObjectsToSaveMap(const ObjectsToSaveMap& o) : HashTable<Reference<DistributedObject*>, Reference<DistributedObjectServant*> >(o) {

	  }
  };

  	class TransactionalObjectManager : public ObjectBroker, public Command {
  		DistributedObjectBroker* objectBroker;

  		ThreadLocal<LocalObjectManager*> localObjectManager;

  		ObjectsToSaveMap implementationCopiesToSave;
  		Mutex saveMutex;

  	public:
  		TransactionalObjectManager();

  		void initialize();

  		void execute();

  		void undo();

		void registerClass(const String& name, DistributedObjectClassHelper* helper);

		// deployment methods
		void deploy(DistributedObjectStub* obj);
		void deploy(const String& name, DistributedObjectStub* obj);

		void requestServant(DistributedObjectStub* obj) {

		}

		DistributedObjectStub* undeploy(const String& name);

		Reference<DistributedObject*> lookUp(const String& name);
		Reference<DistributedObject*> lookUp(uint64 objid);

		bool destroyObject(DistributedObjectStub* obj);

		void addObjectsToSave(const Vector<Reference<Object*> >& objects);

		ObjectsToSaveMap* getModifiedObjectsToSave();

		uint64 getNextFreeObjectID() {
			return objectBroker->getNextFreeObjectID();
		}

		DOBObjectManager* getObjectManager() {
			return objectBroker->getObjectManager();
		}

  	protected:
		LocalObjectManager* getLocalObjectManager();
  	};

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* TRANSACTIONALOBJECTMANAGER_H_ */
