/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ServiceMessageHandlerThread.h"

ServiceMessageHandlerThread::ServiceMessageHandlerThread(const String& s) : ServiceThread(s) {
	socket = NULL;

	clients = NULL;

	serviceHandler = NULL;

	serviceFilter = NULL;
}

ServiceMessageHandlerThread::~ServiceMessageHandlerThread() {
	if (socket != NULL) {
		delete socket;
		socket = NULL;
	}

	if (clients != NULL) {
		delete clients;
		clients = NULL;
	}

	// FIXME: temp hack
	/*if (serviceHandler != NULL && serviceHandler != (ServiceHandler*) this) {
		delete serviceHandler;
		serviceHandler = NULL;
	}*/
}

bool ServiceMessageHandlerThread::removeConnection(ServiceClient* client) {
	if (serviceHandler != NULL)
		serviceHandler->deleteConnection(client);

	Locker locker(this);
	
	return clients->remove(client);
}

void ServiceMessageHandlerThread::removeConnections() {
	if (clients == NULL)
		return;

	clients->resetIterator();

	while (clients->hasNext()) {
		ServiceClient* client = clients->next();

		//delete client;
		client->release();
	}

	clients->removeAll();
}
