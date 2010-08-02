/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DatagramSession.h"

DatagramSession::DatagramSession() : ServiceSession() {
	socket = NULL;
}

DatagramSession::~DatagramSession() {
	socket = NULL;
}

void DatagramSession::bind(const SocketAddress& addr) {
	socket = new DatagramServerSocket(addr);

	ServiceSession::bind(socket->getImplementation(), addr);
}

void DatagramSession::run() {
}

bool DatagramSession::send(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;
	
	socket->send(pack, address);
	
	return true;
}

bool DatagramSession::read(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	SocketAddress addr;
	socket->read(pack, &addr);

	return true;
}
