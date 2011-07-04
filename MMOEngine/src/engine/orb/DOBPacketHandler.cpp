/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DistributedObjectBroker.h"

#include "packets/DeployObjectMessage.h"
#include "packets/LookUpObjectMessage.h"
#include "packets/LookUpObjectResponseMessage.h"
#include "packets/LoadPersistentObjectMessage.h"
#include "packets/UpdatePersistentObjectMessage.h"
#include "packets/UpdatePersistentObjectResponseMessage.h"
#include "packets/GetNextFreeObjectIDMessage.h"
#include "packets/GetNextFreeObjectIDResponseMessage.h"


#include "DOBPacketHandler.h"
#include "DOBObjectManager.h"

DOBPacketHandler::DOBPacketHandler() : Logger() {
	orb = NULL;
}

DOBPacketHandler::DOBPacketHandler(const String& s, DistributedObjectBroker* broker) : Logger(s) {
	orb = broker;
}

DOBPacketHandler::~DOBPacketHandler() {
}

void DOBPacketHandler::handlePacket(DistributedObjectBrokerClient* client, Packet* pack) {
	//System::out << "READ " << pack->toString() << "\n";

	uint32 opcode = pack->parseInt();

	switch (opcode) {
	case 01:
		handleObjectLookUpMessage(client, pack);
		break;

	case 02:
		handleObjectLookUpMessageByID(client, pack);
		break;

	case 03:
		handleObjectDeployMessage(client, pack);
		break;

	case 05:
		handleMethodInvocationMessage(client, pack);
		break;

	case 07:
		handleLoadPersistentObjectMessage(client, pack);
		break;

	case 0x9:
		handleUpdatePersistentObjectMessage(client, pack);
		break;

	case 0x0B:
		handleGetNextFreeObjectIDMessage(client, pack);
		break;
	}
}

void DOBPacketHandler::handleObjectLookUpMessage(DistributedObjectBrokerClient* client, Packet* pack) {
	String name;
	LookUpObjectMessage::parseObjectName(pack, name);

	DistributedObject* obj = orb->lookUp(name);

	Packet* msg = new LookUpObjectResponseMessage(obj);
	client->send(msg);
}

void DOBPacketHandler::handleObjectLookUpMessageByID(DistributedObjectBrokerClient* client, Packet* pack) {
	uint64 objectid = LookUpObjectMessage::parseObjectID(pack);

	DOBObjectManager* objectManager = orb->getObjectManager();

	DistributedObject* obj = objectManager->getObject(objectid);

	Packet* msg = new LookUpObjectResponseMessage(obj);
	client->send(msg);
}

void DOBPacketHandler::handleObjectDeployMessage(DistributedObjectBrokerClient* client, Packet* pack) {
	String name;
	pack->parseAscii(name);

	String classname;
	pack->parseAscii(classname);

	uint64 objid = 0;

	/*ORBClassHelper* helper = classMap.get(classname);
	if (helper != NULL) {


		orb->deploy()
	}

	Packet* msg = new DeployObjectResponseMessage(objid);
	client->send(msg);*/
}

void DOBPacketHandler::handleMethodInvocationMessage(DistributedObjectBrokerClient* client, Packet* pack) {
	uint64 objectid = pack->parseLong();

	DistributedObjectAdapter* adapter = orb->getObjectAdapter(objectid);
	if (adapter == NULL) {
		error("object not found for method invocation");
		return;
	}

	uint32 methodID = pack->parseInt();
	uint32 invocationID = pack->parseInt();

	DistributedMethod invocation(orb, pack);

	Packet* resp = adapter->invokeMethod(methodID, &invocation);
	client->send(resp);
}

void DOBPacketHandler::handleLoadPersistentObjectMessage(DistributedObjectBrokerClient* client, Packet* pack) {
	uint64 objectid = LoadPersistentObjectMessage::parseObjectID(pack);

	DOBObjectManager* objectManager = orb->getObjectManager();

	DistributedObject* obj = objectManager->loadPersistentObject(objectid);

	Packet* msg = new LookUpObjectResponseMessage(obj);
	client->send(msg);
}

void DOBPacketHandler::handleUpdatePersistentObjectMessage(DistributedObjectBrokerClient* client, Packet* pack) {
	uint64 objectid = UpdatePersistentObjectMessage::parseObjectID(pack);

	DOBObjectManager* objectManager = orb->getObjectManager();

	DistributedObject* obj = objectManager->getObject(objectid);

	int success;

	if (obj != NULL) {
		error("could not locate object in  DOBPacketHandler::handleUpdatePersistentObjectMessage");

		success = 0;
	} else {
		success = objectManager->updatePersistentObject(obj);
	}

	Packet* msg = new UpdatePersistentObjectResponseMessage(success);
	client->send(msg);
}

void DOBPacketHandler::handleGetNextFreeObjectIDMessage(DistributedObjectBrokerClient* client, Packet* pack) {
	bool doLock = GetNextFreeObjectIDMessage::parseDoLock(pack);

	DOBObjectManager* objectManager = orb->getObjectManager();

	uint64 objectID = objectManager->getNextFreeObjectID();

	Packet* msg = new GetNextFreeObjectIDResponseMessage(objectID);
	client->send(msg);
}
