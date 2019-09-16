/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/orb/DistributionException.h"

#include "DOBServiceClient.h"

#include "DeployObjectMessage.h"
#include "InvokeMethodMessage.h"
#include "LookUpObjectMessage.h"
#include "LookUpObjectByIDMessage.h"
#include "UndeployObjectMessage.h"
#include "RequestServantMessage.h"
#include "GetNextFreeObjectIDMessage.h"

RemoteObjectBroker::RemoteObjectBroker(const String& address, int port) : Logger("RemoteObjectBroker") {
	brokerClient = new DOBServiceClient(address, port);
	brokerClient->start();
}

RemoteObjectBroker::RemoteObjectBroker(DOBServiceClient* client) : Logger("RemoteObjectBroker") {
	brokerClient = client;
}

RemoteObjectBroker::~RemoteObjectBroker() {
}

void RemoteObjectBroker::registerClass(const String& name, DistributedObjectClassHelper* helper) {
	fatal("RemoteObjectBroker::registerClass");
}

void RemoteObjectBroker::deploy(DistributedObjectStub* obj) {
	Locker locker(this);

	const String& className = obj->_getClassName();
	fatal(!className.isEmpty()) << "object class name is empty";

	DeployObjectMessage deployMessage(obj->_getName(), className, obj->_getObjectID());

	if (!brokerClient->sendAndAcceptReply(&deployMessage))
		throw DistributionException(obj);

	if (!deployMessage.isDeployed())
		throw ObjectAlreadyDeployedException(obj);

	obj->_setObjectID(deployMessage.getObjectID());
}

void RemoteObjectBroker::deploy(const String& name, DistributedObjectStub* obj) {
	Locker locker(this);

	const String& className = obj->_getClassName();
	fatal(!className.isEmpty()) << "class name is empty";

	DeployObjectMessage deployMessage(name, className, obj->_getObjectID());

	if (!brokerClient->sendAndAcceptReply(&deployMessage))
		throw DistributionException(obj);

	if (!deployMessage.isDeployed())
		throw ObjectAlreadyDeployedException(obj);

	obj->_setObjectID(deployMessage.getObjectID());
}

DistributedObjectStub* RemoteObjectBroker::undeploy(const String& name) {
	Locker locker(this);

	UndeployObjectMessage undeployMessage(name);

	if (!brokerClient->sendAndAcceptReply(&undeployMessage))
		throw DistributionException(name);

	return nullptr;
}

void RemoteObjectBroker::addDeployedObject(DistributedObject* obj) {
	deployedObjects.put(obj);
}

void RemoteObjectBroker::removeDeployedObject(DistributedObject* obj) {
	deployedObjects.drop(obj);
}

Reference<DistributedObject*> RemoteObjectBroker::lookUp(const String& name) {
	Locker locker(this);

	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	LookUpObjectMessage lookupMessage(name);

	if (!brokerClient->sendAndAcceptReply(&lookupMessage))
		throw DistributionException(name);

	if (!lookupMessage.isFound())
		return nullptr;

	DistributedObjectStub* obj = broker->createObjectStub(lookupMessage.getClassName(), name);
	if (obj != nullptr) {
		obj->_setObjectID(lookupMessage.getObjectID());

		broker->deployRemote(name, obj);

		obj->_setObjectBroker(this);
	}

	return obj;
}

void RemoteObjectBroker::requestServant(DistributedObjectStub* obj) {
	Locker locker(this);

	RequestServantMessage message(obj->_getObjectID());

	if (!brokerClient->sendAndAcceptReply(&message))
		throw DistributionException("couldnt receive servant data");
}

Reference<DistributedObject*> RemoteObjectBroker::lookUp(uint64 objid) {
	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	Locker locker(this);

	LookUpObjectByIDMessage lookupMessage(objid);

	if (!brokerClient->sendAndAcceptReply(&lookupMessage))
		throw DistributionException(objid);

	if (!lookupMessage.isFound())
		return nullptr;

	const String& name = lookupMessage.getName();

	DistributedObjectStub* obj = broker->createObjectStub(lookupMessage.getClassName(), name);
	if (obj != nullptr) {
		obj->_setObjectID(objid);

		broker->deployRemote(name, obj);

		obj->_setObjectBroker(this);
	}

	return obj;
}

bool RemoteObjectBroker::destroyObject(DistributedObjectStub* obj) {
	return false;
}

/*int RemoteObjectBroker::updatePersistentObject(DistributedObject* object) {
	*//*Packet* msg = new UpdatePersistentObjectMessage(object);
	client->send(msg);

	Packet resp;
	client->read(&resp);

	return UpdatePersistentObjectResponseMessage::parseReturnValue(&resp);*//*
	return 0;
}*/

/*DistributedObjectAdapter* RemoteObjectBroker::addObject(DistributedObjectStub* object) {
	Locker _locker(this);

	DistributedObjectServant* servant = object->_getImplementation();

	DistributedObjectClassHelper* helper = servant->_getClassHelper();
	DistributedObjectAdapter* adapter = helper->createAdapter(object);

	// we would need to notify Master Object Manager

	return localObjectDirectory.add(object->_getObjectID(), adapter);
}*/

uint64 RemoteObjectBroker::getNextFreeObjectID() {
	Locker locker(this);

	GetNextFreeObjectIDMessage objectIDMessage;

	if (!brokerClient->sendAndAcceptReply(&objectIDMessage))
		throw DistributionException();

	return objectIDMessage.getObjectID();
}

/*void RemoteObjectBroker::updateModifiedObjectsToDatabase() {

}*/

void RemoteObjectBroker::invokeMethod(DistributedMethod& method, bool asyncMethod) {
	Locker locker(this);

	DOBMessage* invocationMessage = method.getInvocationMessage();

	if (!asyncMethod) {
		if (!brokerClient->sendAndAcceptReply(invocationMessage))
			throw DistributionException(method.getObject()->_getName());
	} else {
		brokerClient->send(invocationMessage);
	}
}

