/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DatagramConnector.h"

DatagramConnector::DatagramConnector()
		: ServiceSession() {
	doRun = true;
}

DatagramConnector::DatagramConnector(const String& addr, int port)
		: ServiceSession() {
	socket = new DatagramSocket();
	SocketAddress address(addr, port);

	bind(socket->getImplementation(), address);

	doRun = true;
}

DatagramConnector::~DatagramConnector() {
	if (socket != NULL) {
		socket->close();

		delete socket;
		socket = NULL;
	}
}

void DatagramConnector::recieveMessages() {
	Packet packet;

	while (doRun) {
		try	{
			if (!read(&packet))
				continue;

			handleMessage(&packet);
		} catch (SocketException& e) {
			System::out << e.getMessage() << "\n";

			doRun = false;
		}
	}
}

bool DatagramConnector::send(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	if (socket != NULL)
		socket->send(pack);
	else
		throw SocketException();

	return true;
}

bool DatagramConnector::read(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	return socket->read(pack);
}

