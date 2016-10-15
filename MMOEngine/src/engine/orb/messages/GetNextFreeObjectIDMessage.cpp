//
// Created by Victor Popovici on 15/10/16.
//

#include "GetNextFreeObjectIDMessage.h"

GetNextFreeObjectIDMessage::GetNextFreeObjectIDMessage() : DOBMessage(GETNEXTFREEOBJECTIDMESSAGE, 20) {
}

GetNextFreeObjectIDMessage::GetNextFreeObjectIDMessage(Packet* message) : DOBMessage(message) {
}

void GetNextFreeObjectIDMessage::execute() {
	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	uint64 objectID = broker->getNextFreeObjectID();

	insertLong(objectID);

	//printf("sending next free oid:%d", (int) objectID);
	client->sendReply(this);
}

void GetNextFreeObjectIDMessage::handleReply(Packet* response) {
	objectID = response->parseLong();
}