/*
 * DOBObjectManager.cpp
 *
 *  Created on: 20/08/2009
 *      Author: victor
 */

#ifndef DOBOBJECTMANAGER_CPP_
#define DOBOBJECTMANAGER_CPP_

#include "DOBObjectManager.h"
#include "packets/LoadPersistentObjectMessage.h"
#include "packets/LookUpObjectResponseMessage.h"
#include "packets/LookUpObjectMessage.h"
#include "packets/UpdatePersistentObjectMessage.h"
#include "packets/UpdatePersistentObjectResponseMessage.h"
#include "packets/GetNextFreeObjectIDMessage.h"
#include "packets/GetNextFreeObjectIDResponseMessage.h"


#include "DistributedObjectBrokerClient.h"
#include "DistributedObjectBroker.h"

DOBObjectManager::DOBObjectManager(DistributedObjectBrokerClient* orbClient) {
	client = orbClient;
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
	DistributedObjectServant* servant = object->_getImplementation();

	DistributedObjectClassHelper* helper = servant->_getClassHelper();
	DistributedObjectAdapter* adapter = helper->createAdapter(object);

	// we would need to notify Master Object Manager

	return localObjectDirectory.add(object->_getObjectID(), adapter);
}

DistributedObject* DOBObjectManager::getObject(uint64 objectID) {
	DistributedObject* obj = NULL;

	Locker _locker(this);

	obj = localObjectDirectory.get(objectID);

	if (obj != NULL)
		return obj;
	else {
		//get the object from remote DOB
	}

	/*
	Packet* msg = new LookUpObjectMessage(objectID);
	client->send(msg);

	Packet resp;
	client->read(&resp);

	if (!LookUpObjectResponseMessage::parseObjectFound(&resp))
		return NULL;

	String className;
	resp.parseAscii(className);

	uint64 objectid = resp.parseLong();

	DistributedObjectClassHelperMap* classMap = DistributedObjectBroker::instance()->getClassMap();

	DistributedObjectClassHelper* helper = classMap->get(className);
	if (helper != NULL) {
		obj = helper->instantiateObject();

		obj->_setName(className);
		obj->_setObjectID(objectid);
	}*/

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

uint64 DOBObjectManager::getNextFreeObjectID() {
	Packet* msg = new GetNextFreeObjectIDMessage(true);
	client->send(msg);

	Packet resp;
	client->read(&resp);

	return GetNextFreeObjectIDResponseMessage::parseObjectID(&resp);
}

#endif /* DOBOBJECTMANAGER_CPP_ */
