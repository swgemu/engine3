/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TransactionalSocketManager.h"

TransactionalSocketManager::TransactionalSocketManager() : Logger("TransactionalSocketManager") {
	setLogging(true);
}

void TransactionalSocketManager::sendMessage(Message* message) {
	MessageQueue* queue = getLocalMessageQueue();

	queue->push(message);
}

void TransactionalSocketManager::execute() {
	MessageQueue* queue = getLocalMessageQueue();

	info("sending " + String::valueOf(queue->size()) + " messages");

	for (int i = 0; i < queue->size(); ++i) {
		Message* message = queue->get(i);

		ServiceClient* client = message->getClient();

		Socket* socket = client->getSocket();
		if (socket != NULL) {
				SocketAddress& addr = client->getAddress();

				socket->sendTo(message, &addr);
		}

		delete message;
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

		info("message queue created");

		sentMessages.set(queue);
	}

	return queue;
}
