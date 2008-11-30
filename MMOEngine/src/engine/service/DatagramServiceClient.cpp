/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DatagramServiceClient.h"

DatagramServiceClient::DatagramServiceClient(const String& host, int port) 
		: ServiceClient(host, port) {
	socket = new UDPSocket();

	doRun = true;
}

DatagramServiceClient::DatagramServiceClient(Socket* sock, SocketAddress& addr) 
		: ServiceClient(sock, addr) {
}

DatagramServiceClient::~DatagramServiceClient() {
	close();
}

void DatagramServiceClient::recieveMessages() {
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

bool DatagramServiceClient::send(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	if (socket != NULL)
		socket->sendTo(pack, &addr);
	else
		throw SocketException();

	return true;
}

bool DatagramServiceClient::read(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	SocketAddress addr;

	return socket->recieveFrom(pack, &addr);
}

