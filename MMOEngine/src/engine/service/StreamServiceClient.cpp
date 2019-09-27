/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/log/Logger.h"

#include "ServiceHandler.h"

#include "StreamServiceClient.h"

StreamServiceClient::StreamServiceClient(Socket* sock) : ServiceClient(sock), Thread() {
	doRun = true;
}

StreamServiceClient::StreamServiceClient(Socket* sock, const SocketAddress& addr)
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
	if (socket == nullptr) {
		socket = new TCPSocket();
	}

	socket->connect(&addr);
}

void StreamServiceClient::run() {
	System::err << "[StreamServiceClient] WARNING - client shouldn't run\n";
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

	assert(serviceHandler);

	while (doRun) {
		try	{
			packet.clear();

			if (recieve(&packet)) {
				if (packet.size() == 0)
					break;

				serviceHandler->handleMessage(this, &packet);
			} else {
				break;
			}
		} catch (SocketException& e) {
			if (!serviceHandler->handleError(this, e))
				break;
		}
	}

	disconnect();
}

int StreamServiceClient::send(Packet* pack) {
	if (socket != nullptr) {
		int res = 0;
		try {
			res = socket->send(pack);

			serviceHandler->messageSent(pack);

			return res;
		} catch(SocketException& s) {
			doRun = false;

			throw;
		}
	} else {
		doRun = false;

		throw SocketException();
	}
}

bool StreamServiceClient::read(Packet* pack) {
	return socket->read(pack);
}

bool StreamServiceClient::recieve(Packet* pack) {
	if (socket != nullptr) {
		return socket->read(pack);
	} else {
		doRun = false;

		throw SocketException();
	}
}

bool StreamServiceClient::receiveAppend(Stream* stream) {
	if (socket != nullptr) {
		return socket->readAppend(stream);
	} else {
		doRun = false;

		throw SocketException();
	}
}

void StreamServiceClient::disconnect() {
	close();

	serviceHandler->deleteConnection(this);
}
