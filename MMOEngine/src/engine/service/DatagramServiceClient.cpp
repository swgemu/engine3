/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/core/Core.h"

#include "engine/stm/TransactionalMemoryManager.h"
#include "engine/stm/service/TransactionalSocketManager.h"

#include "DatagramServiceClient.h"

DatagramServiceClient::DatagramServiceClient()
		: ServiceClient() {
	socket = new UDPSocket();

	doRun = true;
}

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

class ClientMessageReceiverTask : public Task {
	Reference<DatagramServiceClient*> client;

	Packet* message;

public:
	ClientMessageReceiverTask(DatagramServiceClient* cli, Packet* packet) {
		client = cli;
		printf("FUCK\n");

		message = packet->clone();
	}

	~ClientMessageReceiverTask() {
		delete message;
		message = NULL;
	}

	void run() {
		client->handleMessage(message);
	}
};

void DatagramServiceClient::recieveMessages() {
	Packet packet;

	while (doRun) {
		try	{
			if (!read(&packet))
				continue;

			Reference<Task*> receiverTask = new ClientMessageReceiverTask(this, &packet);
			receiverTask->execute();
		} catch (SocketException& e) {
			System::out << e.getMessage() << "\n";

			doRun = false;
		}
	}
}

bool DatagramServiceClient::send(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	if (socket != NULL) {
		// FIXME: temp hack for stm
	#ifdef WITH_STM
		Message* message = new Message(pack, this);

		TransactionalMemoryManager::instance()->getSocketManager()->sendMessage(message);
	#else
		socket->sendTo(pack, &addr);
	#endif
	} else
		throw SocketException();

	return true;
}

bool DatagramServiceClient::read(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	SocketAddress addr;

	return socket->recieveFrom(pack, &addr);
}

