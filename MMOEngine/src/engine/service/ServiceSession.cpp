/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ServiceFilterChain.h"

#include "ServiceSession.h"

ServiceSession::ServiceSession() {
	packetLossChance = 0;

	filterChain = new ServiceFilterChain();

	handler = NULL;
}

ServiceSession::~ServiceSession() {
	if (filterChain != NULL) {
		delete filterChain;
		filterChain = NULL;
	}

	if (handler != NULL) {
		delete handler;
		handler = NULL;
	}
}

void ServiceSession::bind(SocketImplementation* sock, const SocketAddress& addr) {
	socket = sock;

	address = addr;
}

bool ServiceSession::isAvailable() {
	return !(hasError || disconnected);			
}

void ServiceSession::finalize() {
	release();
}

void ServiceSession::acquire() {
	Object::acquire();
}

void ServiceSession::release() {
	Object::release();
}
