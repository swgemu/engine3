/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TransactionalSocketManager.h"

TransactionalSocketManager::TransactionalSocketManager() : Logger("TransactionalSocketManager") {
	setLogging(false);
	setGlobalLogging(false);
}

void TransactionalSocketManager::sendMessage(Message* message) {
	MessageQueue* queue = getLocalMessageQueue();

	//info("queuing message");

	message->acquire();

	queue->push(message);
}

void TransactionalSocketManager::execute() {
	MessageQueue* queue = getLocalMessageQueue();

	/*if (queue->size() > 0)
		info("sending " + String::valueOf(queue->size()) + " messages");*/

	for (int i = 0; i < queue->size(); ++i) {
		Message* message = queue->get(i);

		ServiceClient* client = message->getClient();

		Socket* socket = client->getSocket();
		if (socket != NULL) {
				//info("SEND(RAW) - " + message->toStringData(), true);

				SocketAddress& addr = client->getAddress();

				socket->sendTo(message, &addr);
		}

		message->release();
	}

	queue->removeAll();
}

void TransactionalSocketManager::undo() {
	MessageQueue* queue = getLocalMessageQueue();

	for (int i = 0; i < queue->size(); ++i) {
		Message* message = queue->get(i);

		delete message;
	}

	queue->removeAll();
}

MessageQueue* TransactionalSocketManager::getLocalMessageQueue() {
	MessageQueue* queue = sentMessages.get();

	if (queue == NULL) {
		queue = new MessageQueue();

		//info("message queue created");

		sentMessages.set(queue);
	}

	return queue;
}
