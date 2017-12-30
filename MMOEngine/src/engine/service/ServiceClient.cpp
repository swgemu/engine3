/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ServiceClient.h"

ServiceClient::ServiceClient() {
	socket = nullptr;

	errored = false;

	packetLossChance = 0;
}

ServiceClient::ServiceClient(Socket* sock) {
	socket = sock;

	errored = false;

	packetLossChance = 0;
}

ServiceClient::ServiceClient(Socket* sock, SocketAddress& addr) {
	socket = sock;
	ServiceClient::addr = addr;

	errored = false;

	packetLossChance = 0;
}

ServiceClient::ServiceClient(const String& host, int port) {
	setAddress(host, port);

	errored = false;

	packetLossChance = 0;
}

ServiceClient::~ServiceClient() {
	//StackTrace::printStackTrace();
}

void ServiceClient::close() {
	if (socket != nullptr) {
		socket->close();
		
		delete socket;
		socket = nullptr;
	}			
}

bool ServiceClient::isAvailable() {
	if (socket == nullptr)
		return false;
	
	return !(errored || disconnected);
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
