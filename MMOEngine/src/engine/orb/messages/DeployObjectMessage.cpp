/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 15/10/16.
//

#include "DeployObjectMessage.h"

DeployObjectMessage::DeployObjectMessage(const String& name, const String& classname,
					 uint64 requestedObjectID) : DOBMessage(DEPLOYOBJECTMESSAGE, 40),
					 requestedObjectID(requestedObjectID), deployed(false) {
	insertAscii(name);
	insertAscii(classname);
	insertLong(requestedObjectID);
}

DeployObjectMessage::DeployObjectMessage(Packet* message) : DOBMessage(message) {
	message->parseAscii(name);
	message->parseAscii(className);
	requestedObjectID = message->parseLong();

	deployed = false;
	objectID = 0;
}

void DeployObjectMessage::execute() {
	//printf("Received DeployObjectMessage with oid:%d\n", (int) requestedObjectID);

	DistributedObjectBroker* broker = DistributedObjectBroker::instance();
	RemoteObjectBroker* remoteBroker = client->getRemoteObjectBroker();

	DistributedObjectStub* obj = broker->createObjectStub(className, name);

	if (obj != nullptr) {
		try {
			//printf("Received DeployObjectMessage with name:%s\n", obj->_getName().toCharArray());

			obj->_setObjectID(requestedObjectID);
			broker->deployLocal(obj->_getName(), obj);

			obj->_setObjectBroker(remoteBroker);

			insertBoolean(true);
			insertLong(obj->_getObjectID());

			//printf("returning oid in DeployObjectMessage:%d\n", obj->_getObjectID());

			remoteBroker->addDeployedObject(obj);
		} catch (const Exception& e) {
			e.printStackTrace();

			delete obj;

			insertBoolean(false);
		}
	} else {
		insertBoolean(false);
	}

	client->sendReply(this);
}

void DeployObjectMessage::handleReply(Packet* message) {
	//printf("Received response to DeployObjectMessage\n");

	deployed = message->parseBoolean();

	if (deployed) {
		objectID = message->parseLong();

		//printf("successful response to DeployObjectMessage with oid:%d\n", (int)objectID);
	}
}
