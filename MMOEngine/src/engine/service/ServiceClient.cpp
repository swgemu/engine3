/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

ServiceClient::ServiceClient(Socket* sock, const SocketAddress& addr) {
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
