/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/orb/DistributedObjectBroker.h"
#include "engine/orb/DOBObjectManager.h"

#include "engine/stm/TransactionAbortedException.h"

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

	info("deployed " + String::valueOf(localObjectDirectory.size())
			+ ", undeployed " + String::valueOf(undeployedObjects.size())
			+ ", destroyed " + String::valueOf(destroyedObjects.size()));

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

//HashTable<uint64,StackTrace*> traces;

void LocalObjectManager::deploy(const String& name, DistributedObjectStub* obj) {
	objectManager->createObjectID(name, obj);

	const String& objectName = obj->_getName();

	assert(!objectName.isEmpty());

	if (lookUp(objectName) != NULL) {
		warning("object \'" + objectName + "\' (0x" + String::valueOf(obj) + ") already deployed");

		/*traces.get(obj->_getObjectID())->print();

		printf("new\n");

		StackTrace::printStackTrace();

		assert(0);*/

		throw TransactionAbortedException();
	} else {
		//traces.put(obj->_getObjectID(), new StackTrace());

		assert(localNamingDirectory.put(objectName, obj) == NULL);

		assert(localObjectDirectory.put(obj->_getObjectID(), obj) == NULL);

		info("object " + objectName + " deployed");

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
	DistributedObject* object = localObjectDirectory.get(objid);
	if (object == NULL)
		object = objectBroker->lookUp(objid);

	if (object != NULL && undeployedObjects.contains((DistributedObjectStub*) object))
		return NULL;

	return object;
}

bool LocalObjectManager::destroyObject(DistributedObjectStub* obj) {
	destroyedObjects.put(obj);

	return true;
}
