/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../log/Logger.h"

#include "StreamConnector.h"

StreamConnector::StreamConnector() : ServiceSession(), Thread() {
	socket = new StreamSocket();

	doRun = true;
}

StreamConnector::~StreamConnector() {
	if (socket != NULL) {
		socket->close();

		delete socket;
		socket = NULL;
	}
}

void StreamConnector::connect(const SocketAddress& addr) {
	address = addr;

	socket->connect(address);
}

void StreamConnector::run() {
	System::out << "[StreamServiceClient] WARNING - client shouldn't run\n"; 
}

void StreamConnector::start() {
	doRun = true;
	
	Thread::start();
}

void StreamConnector::stop() {
	doRun = false;
	
	Thread::join();
	
	finalize();
}

void StreamConnector::receiveMessages() {
	Packet packet;

	while (doRun) {
		try	{
			packet.clear();


			if (recieve(&packet)) {
				if (packet.size() == 0)
					break;

				handleMessage(&packet);
			}
		} catch (SocketException& e) {
			if (!handleError(e))
				break;
		}
	}

	disconnect();
}

bool StreamConnector::handleError(Exception& e) {
	Logger::console.info("(" + address.getFullIPAddress() + ") - " + e.getMessage(), true);

	return false;
}

bool StreamConnector::send(Packet* pack) {
	if (socket != NULL) {
		socket->send(pack);
	} else {
		doRun = false;

		throw SocketException();
	}

	return true;
}

bool StreamConnector::read(Packet* pack) {
	return socket->read(pack);
}

bool StreamConnector::recieve(Packet* pack) {
	if (socket != NULL) {
		return socket->read(pack);
	} else {
		doRun = false;

		throw SocketException();
	}
}

void StreamConnector::disconnect() {
	if (socket != NULL) {
		socket->close();

		delete socket;
		socket = NULL;
	}
}
