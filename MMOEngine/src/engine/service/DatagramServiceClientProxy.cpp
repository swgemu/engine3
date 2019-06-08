/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "DatagramServiceClientProxy.h"

DatagramServiceClientProxy::DatagramServiceClientProxy(Socket* sock, SocketAddress& addr) 
		: ServiceClient(sock, addr) {
}

DatagramServiceClientProxy::~DatagramServiceClientProxy() {
	socket = nullptr;
}

void DatagramServiceClientProxy::run() {
}

int DatagramServiceClientProxy::send(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return -1;
	
	return socket->sendTo(pack, &addr);
}

bool DatagramServiceClientProxy::read(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	SocketAddress addr;
	socket->readFrom(pack, &addr);

	return true;
}
