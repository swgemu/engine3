/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/log/Logger.h"

#include "ServiceHandler.h"

#include "StreamServiceClient.h"

StreamServiceClient::StreamServiceClient(Socket* sock) : ServiceClient(sock), Thread() {
	doRun = true;
}

StreamServiceClient::StreamServiceClient(Socket* sock, SocketAddress& addr) 
		: ServiceClient(sock, addr), Thread() {
	doRun = true;
}

StreamServiceClient::StreamServiceClient(const String& host, int port) 
		: ServiceClient(host, port), Thread() {
	socket = new TCPSocket();

	doRun = true;
}

StreamServiceClient::~StreamServiceClient() {
	close();
}

void StreamServiceClient::connect() {
	if (socket == NULL)
	{
		socket = new TCPSocket();
	}

	socket->connect(&addr);
}

void StreamServiceClient::run() {
	System::out << "[StreamServiceClient] WARNING - client shouldn't run\n"; 
}

void StreamServiceClient::start() {
	doRun = true;
	
	Thread::start();
}

void StreamServiceClient::stop() {
	doRun = false;
	
	Thread::join();
	
	finalize();
}

void StreamServiceClient::receiveMessages() {
	Packet packet;

	socket->setTimeOut(1000);

	assert(serviceHandler);

	while (doRun) {
		try	{
			packet.clear();

			if (recieve(&packet)) {
				if (packet.size() == 0)
					break;

				serviceHandler->handleMessage(this, &packet);
			}
		} catch (SocketException& e) {
			if (!serviceHandler->handleError(this, e))
				break;
		}
	}

	disconnect();
}

int StreamServiceClient::send(Packet* pack) {
	if (socket != NULL) {
		int res = socket->send(pack);

		serviceHandler->messageSent(pack);

		return res;
	} else {
		doRun = false;

		throw SocketException();
	}
}

bool StreamServiceClient::read(Packet* pack) {
	return socket->read(pack);
}

bool StreamServiceClient::recieve(Packet* pack) {
	if (socket != NULL) {
		return socket->recieve(pack);
	} else {
		doRun = false;

		throw SocketException();
	}
}

void StreamServiceClient::disconnect() {
	close();
}
