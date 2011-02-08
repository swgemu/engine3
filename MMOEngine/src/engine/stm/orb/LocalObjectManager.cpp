/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/orb/DistributedObjectBroker.h"

#include "LocalObjectManager.h"

LocalObjectManager::LocalObjectManager() : Logger("LocalObjectManager") {
	objectBroker = DistributedObjectBroker::instance();

	namingDirectory = new NamingDirectoryServiceImpl();

	setLogging(true);
	setGlobalLogging(true);
}

void LocalObjectManager::commitObjectChanges() {
	HashTableIterator<String,DistributedObjectStub*> iterator = namingDirectory->getObjects();

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
	namingDirectory->clearNameMap();

	undeployedObjects.removeAll();
	destroyedObjects.removeAll();
}

void LocalObjectManager::registerClass(const String& name, DistributedObjectClassHelper* helper) {
}

void LocalObjectManager::deploy(DistributedObjectStub* obj) {
	if (!namingDirectory->deploy(obj)) {
		warning("object already deployed");
	} else {
		info("object " + String::valueOf(obj->_getObjectID()) + " deployed");

		undeployedObjects.drop(obj);
	}
}

void LocalObjectManager::deploy(const String& name, DistributedObjectStub* obj) {
	if (!namingDirectory->deploy(name, obj)) {
		warning("object \'" + name + "\'  already deployed");
	} else {
		info("object " + name + " deployed");

		undeployedObjects.drop(obj);
	}
}

DistributedObjectStub* LocalObjectManager::undeploy(const String& name) {
	DistributedObjectStub * object = (DistributedObjectStub*) namingDirectory->undeploy(name);
	if (object != NULL)
		undeployedObjects.put(object);

	return object;
}

DistributedObject* LocalObjectManager::lookUp(const String& name) {
	DistributedObject* object = namingDirectory->lookUp(name);
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
