/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/orb/DistributedObjectBroker.h"
#include "engine/orb/DOBObjectManager.h"


#include "LocalObjectManager.h"

LocalObjectManager::LocalObjectManager() : Logger("LocalObjectManager"),
		localObjectDirectory(100000), localNamingDirectory(100000) {
	objectBroker = DistributedObjectBroker::instance();

	objectManager = objectBroker->getObjectManager();

	setLogging(false);
	setGlobalLogging(true);
}

void LocalObjectManager::commitObjectChanges() {
	HashTableIterator<uint64,DistributedObjectStub*> iterator = localObjectDirectory.iterator();

	while (iterator.hasNext()) {
		DistributedObjectStub* object = iterator.next();

		objectBroker->deploy(object);
	}

	for (int i = 0; i < undeployedObjects.size(); ++i) {
		DistributedObjectStub* object = undeployedObjects.get(i);

		objectBroker->undeploy(object->_getName());
	}

	for (int i = 0; i < destroyedObjects.size(); ++i) {
		DistributedObjectStub* object = destroyedObjects.get(i);

		objectBroker->destroyObject(object);
	}

	clearObjectChanges();
}

void LocalObjectManager::clearObjectChanges() {
	localObjectDirectory.removeAll();
	localNamingDirectory.removeAll();

	undeployedObjects.removeAll();
	destroyedObjects.removeAll();
}

void LocalObjectManager::registerClass(const String& name, DistributedObjectClassHelper* helper) {
}

void LocalObjectManager::deploy(DistributedObjectStub* obj) {
	String name = obj->_getName();

	objectManager->createObjectID(name, obj);

	deploy(name, obj);
}

void LocalObjectManager::deploy(const String& name, DistributedObjectStub* obj) {
	if (lookUp(name) != NULL) {
		warning("object \'" + name + "\' (0x" + String::valueOf(obj) + ") already deployed");
	} else {
		objectManager->createObjectID(name, obj);

		assert(localNamingDirectory.put(name, obj) == NULL);

		assert(localObjectDirectory.put(obj->_getObjectID(), obj) == NULL);

		info("object " + name + " deployed");

		undeployedObjects.drop(obj);
	}
}

DistributedObjectStub* LocalObjectManager::undeploy(const String& name) {
	DistributedObjectStub * object = localNamingDirectory.get(name);
	if (object != NULL)
		undeployedObjects.put(object);

	return object;
}

DistributedObject* LocalObjectManager::lookUp(const String& name) {
	DistributedObject* object = localNamingDirectory.get(name);
	if (object == NULL)
		object = objectBroker->lookUp(name);

	if (object != NULL && !undeployedObjects.contains((DistributedObjectStub*) object))
		return object;
	else
		return NULL;
}

DistributedObject* LocalObjectManager::lookUp(uint64 objid) {
	DistributedObject* object = NULL; //namingDirectory->lookUp(objid);
	//if (object == NULL && !undeployedObjects.contains(objid))
		object = objectBroker->lookUp(objid);

	return object;
}

bool LocalObjectManager::destroyObject(DistributedObjectStub* obj) {
	destroyedObjects.put(obj);

	return true;
}
