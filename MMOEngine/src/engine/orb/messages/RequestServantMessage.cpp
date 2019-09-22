/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 15/10/16.
//

#include "RequestServantMessage.h"

#include "engine/core/ManagedObject.h"
#include "RemoteObjectBroker.h"

RequestServantMessage::RequestServantMessage(uint64 oid) : DOBMessage(REQUESTSERVANT, 40), objectID(oid) {
	insertLong(oid);
}

RequestServantMessage::RequestServantMessage(Packet* message) : DOBMessage(message) {
	objectID = message->parseLong();
}

void RequestServantMessage::execute() {
	//printf("RequestServantMessage::execute()\n");

	Reference<ManagedObject*> obj = DistributedObjectBroker::instance()->lookUp(objectID).castTo<ManagedObject*>();

	insertLong(objectID);

	if (obj != nullptr) {
		Locker locker(obj);

		ObjectOutputStream stream;

		obj->writeObject(&stream);

		locker.release();

		RemoteObjectBroker* remoteBroker = client->getRemoteObjectBroker();
		RemoteObjectBroker* oldBroker = dynamic_cast<RemoteObjectBroker*>(obj->_getObjectBroker());

		obj->_setObjectBroker(remoteBroker);
		obj->_setImplementation(nullptr);

		if (remoteBroker != nullptr) {
			remoteBroker->addDeployedObject(obj);
		}

		if (oldBroker != nullptr) {
			oldBroker->removeDeployedObject(obj);
		}

		insertBoolean(true);
		insertAscii(obj->_getClassName());
		insertInt(stream.size());
		insertStream(&stream);
	} else {
		insertBoolean(false);
	}

	client->sendReply(this);
}

void RequestServantMessage::handleReply(Packet* message) {
	//printf("received servant data:%s\n", message->toStringData().toCharArray());

	objectID = message->parseLong();
	bool available = message->parseBoolean();

	if (available) {
		String className;
		message->parseAscii(className);

		int size = message->parseInt();

		ObjectInputStream stream;
		message->parseStream(&stream, size);

		stream.reset();

		Reference<ManagedObject*> obj = DistributedObjectBroker::instance()->lookUp(
				objectID).castTo<ManagedObject*>();

		if (obj != nullptr) {
			DistributedObjectBroker::instance()->createObjectServant(className, obj, true);

			E3_ASSERT(obj->_getImplementation());

			Locker locker(obj);

			obj->readObject(&stream);
		}
	}
}

