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

	if (obj != NULL) {
		Locker locker(obj);

		ObjectOutputStream stream;

		obj->writeObject(&stream);

		locker.release();

		obj->_setObjectBroker(client->getRemoteObjectBroker());
		obj->_setImplementation(NULL);

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

	String className;

	objectID = message->parseLong();
	bool available = message->parseBoolean();

	if (available) {
		message->parseAscii(className);

		int size = message->parseInt();

		ObjectInputStream stream;
		message->parseStream(&stream, size);

		stream.reset();

		Reference<ManagedObject*> obj = DistributedObjectBroker::instance()->lookUp(
				objectID).castTo<ManagedObject*>();

		if (obj != NULL) {
			DistributedObjectBroker::instance()->createObjectServant(className, obj, true);

			assert(obj->_getImplementation());

			Locker locker(obj);

			obj->readObject(&stream);
		}
	}
}

