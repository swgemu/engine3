//
// Created by Victor Popovici on 15/10/16.
//

#include "DeployObjectMessage.h"

DeployObjectMessage::DeployObjectMessage(const String& name, const String& classname,
										 uint64 requestedObjectID) : DOBMessage(DEPLOYOBJECTMESSAGE, 40), requestedObjectID(requestedObjectID) {
	insertAscii(name);
	insertAscii(classname);
	insertLong(requestedObjectID);
}

DeployObjectMessage::DeployObjectMessage(Packet* message) : DOBMessage(message) {
	message->parseAscii(name);
	message->parseAscii(className);
	requestedObjectID = message->parseLong();
}

void DeployObjectMessage::execute() {
	//printf("Received DeployObjectMessage\n");

	DistributedObjectBroker* broker = DistributedObjectBroker::instance();
	ObjectBroker* remoteBroker = static_cast<ObjectBroker*>(client->getRemoteObjectBroker());

	DistributedObjectStub* obj = broker->createObjectStub(className, name);
	if (obj != NULL) {
		try {
			obj->_setObjectID(requestedObjectID);
			broker->deployLocal(obj->_getName(), obj);

			obj->_setObjectBroker(remoteBroker);

			insertBoolean(true);
			insertLong(obj->_getObjectID());
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