/*
 * DOBObjectManager.cpp
 *
 *  Created on: 20/08/2009
 *      Author: victor
 */

#ifndef DOBOBJECTMANAGER_CPP_
#define DOBOBJECTMANAGER_CPP_

#include "DOBObjectManager.h"

#include "engine/orb/DistributedObjectBroker.h"

DOBObjectManager::DOBObjectManager() {
	nextObjectID = 0x1000000; // reserving first ids
}

int DOBObjectManager::updatePersistentObject(DistributedObject* object) {
	/*Packet* msg = new UpdatePersistentObjectMessage(object);
	client->send(msg);

	Packet resp;
	client->read(&resp);

	return UpdatePersistentObjectResponseMessage::parseReturnValue(&resp);*/
	return 0;
}

DistributedObjectAdapter* DOBObjectManager::addObject(DistributedObjectStub* object) {
	DistributedObjectServant* servant = object->getServant();

	if (servant == NULL) {
		// object not local we add it to remote DOB <-> hosts map
		return NULL;
	}

	Locker _locker(this);

	DistributedObjectClassHelper* helper = servant->_getClassHelper();
	DistributedObjectAdapter* adapter = helper->createAdapter(object);

	return localObjectDirectory.add(object->_getObjectID(), adapter);
}

DistributedObject* DOBObjectManager::getObject(uint64 objectID) {
	DistributedObject* obj = NULL;

	Locker _locker(this);

	obj = localObjectDirectory.get(objectID);

	if (obj != NULL)
		return obj;
	else {
		//get the object from the remote DOB map
	}

	return obj;
}

DistributedObjectAdapter* DOBObjectManager::getAdapter(uint64 objectID) {
	Locker _locker(this);

	DistributedObjectAdapter* adapter =  localObjectDirectory.getAdapter(objectID);

	return adapter;
}

DistributedObjectAdapter* DOBObjectManager::removeObject(uint64 objectID) {
	Locker _locker(this);

	DistributedObjectAdapter* object = localObjectDirectory.remove(objectID);

	return object;
}

void DOBObjectManager::createObjectID(const String& name, DistributedObjectStub* object) {
	Locker _locker(this);

	uint64 objectid = object->_getObjectID();

	if (objectid == 0) {
		objectid = getNextFreeObjectID();
		object->_setObjectID(objectid);
	}

	if (name.isEmpty()) {
		/*if (object->_getName().isEmpty())*/ {
			StringBuffer orbname;
			orbname << "_OrbObject" << objectid;

			object->_setName(orbname.toString());
		}
	} else
		object->_setName(name);
}

uint64 DOBObjectManager::getNextFreeObjectID() {
	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	if (!broker->isRootBroker())
		return broker->getNextFreeObjectID();

	Locker _locker(this);

	uint64 val = ++nextObjectID;

	return val;
}

/*void DOBObjectManager::savePersistentObjects() {
	Locker locker(this);

	localObjectDirectory.savePersistentObjects();
}*/

void DOBObjectManager::updateModifiedObjectsToDatabase() {

}

#endif /* DOBOBJECTMANAGER_CPP_ */
