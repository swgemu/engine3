/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "ServiceMessageHandlerThread.h"

ServiceMessageHandlerThread::ServiceMessageHandlerThread(const String& s) : ServiceThread(s) {
	socket = nullptr;

	clients = nullptr;

	serviceHandler = nullptr;

	serviceFilter = nullptr;
}

ServiceMessageHandlerThread::~ServiceMessageHandlerThread() {
	if (socket != nullptr) {
		delete socket;
		socket = nullptr;
	}

	if (clients != nullptr) {
		delete clients;
		clients = nullptr;
	}

	// FIXME: temp hack
	/*if (serviceHandler != nullptr && serviceHandler != (ServiceHandler*) this) {
		delete serviceHandler;
		serviceHandler = nullptr;
	}*/
}

bool ServiceMessageHandlerThread::removeConnection(ServiceClient* client) {
	if (serviceHandler != nullptr)
		serviceHandler->deleteConnection(client);

	Locker locker(this);
	
	return clients->remove(client);
}

void ServiceMessageHandlerThread::removeConnections() {
	if (clients == nullptr)
		return;

	clients->resetIterator();

	while (clients->hasNext()) {
		ServiceClient* client = clients->next();

		//delete client;
		client->release();
	}

	clients->removeAll();
}
