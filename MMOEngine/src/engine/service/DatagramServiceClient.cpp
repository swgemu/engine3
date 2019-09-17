/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/core/Core.h"

#include "engine/stm/TransactionalMemoryManager.h"

#include "DatagramServiceClient.h"

DatagramServiceClient::DatagramServiceClient()
		: ServiceClient() {
	socket = new UDPSocket();

	serviceHandler = nullptr;

	doRun = true;
}

DatagramServiceClient::DatagramServiceClient(const String& host, int port)
		: ServiceClient(host, port) {
	socket = new UDPSocket();

	serviceHandler = nullptr;

	doRun = true;
}

DatagramServiceClient::DatagramServiceClient(Socket* sock, const SocketAddress& addr)
		: ServiceClient(sock, addr) {
	serviceHandler = nullptr;
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

		Logger::console.info("message received");

		message = packet->clone();
	}

	~ClientMessageReceiverTask() {
		delete message;
		message = nullptr;
	}

	void run() {
		if (client->isAvailable())
			client->handleMessage(message);
	}
};

void DatagramServiceClient::recieveMessages() {
	Packet packet;

	Logger::console.info("recieving messages");

	while (doRun) {
		try	{
			if (!read(&packet))
				continue;

			if (!isAvailable())
				return;

			Reference<Task*> receiverTask = new ClientMessageReceiverTask(this, &packet);
			receiverTask->executeInThread();
		} catch (SocketException& e) {
			System::out << e.getMessage() << "\n";

			doRun = false;
		}
	}
}

void DatagramServiceClient::handleMessage(Packet* message) {
	serviceHandler->handleMessage(this, message);
}

int DatagramServiceClient::send(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	if (socket != nullptr) {
		// FIXME: temp hack for stm
	#ifdef WITH_STM
		Message* message = new Message(pack, this);

		TransactionalMemoryManager::instance()->getSocketManager()->sendMessage(message);

		return pack->size();
	#else
		return socket->sendTo(pack, &addr);
	#endif
	} else
		throw SocketException("socket is null");
}

bool DatagramServiceClient::read(Packet* pack) {
	if (packetLossChance != 0 && System::random(100) < (uint32) packetLossChance)
		return false;

	SocketAddress addr;

	if (socket)
		return socket->readFrom(pack, &addr);
	else
		return false;
}

