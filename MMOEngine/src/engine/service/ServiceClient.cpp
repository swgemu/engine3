/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ServiceClient.h"

ServiceClient::ServiceClient() {
	socket = NULL;

	packetLossChance = 0;
}

ServiceClient::ServiceClient(Socket* sock) {
	socket = sock;

	packetLossChance = 0;
}

ServiceClient::ServiceClient(Socket* sock, SocketAddress& addr) {
	socket = sock;
	ServiceClient::addr = addr;

	packetLossChance = 0;
}

ServiceClient::ServiceClient(const String& host, int port) {
	setAddress(host, port);
	
	packetLossChance = 0;
}

ServiceClient::~ServiceClient() {
}

void ServiceClient::close() {
	if (socket != NULL) {
		socket->close();
		
		delete socket;
		socket = NULL;
	}			
}

bool ServiceClient::isAvailable() {
	if (socket == NULL)
		return false;
	
	return !(hasError || disconnected);			
}

void ServiceClient::finalize() {
	release();
}

void ServiceClient::acquire() {
	Object::acquire();
}

void ServiceClient::release() {
	Object::release();
}
