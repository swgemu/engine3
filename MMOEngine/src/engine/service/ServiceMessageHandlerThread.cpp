/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ServiceMessageHandlerThread.h"

ServiceMessageHandlerThread::ServiceMessageHandlerThread(const string& s) : ServiceThread(s) {
	socket = NULL;
	clients = NULL;
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
}

bool ServiceMessageHandlerThread::deleteConnection(ServiceClient* client) {
	lock();
	
	bool res = clients->remove(client);
	
	unlock();
	return res;
}
