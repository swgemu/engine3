/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/io/IOProcessor.h"
#include "system/io/IOHandler.h"

#include "engine/core/Core.h"
#include "engine/core/Task.h"

#include "DatagramServiceThread.h"

DatagramServiceThread::DatagramServiceThread() : ServiceMessageHandlerThread("") {
	setInfoLogLevel();
}

DatagramServiceThread::DatagramServiceThread(const String& s) : ServiceMessageHandlerThread(s) {
	setInfoLogLevel();
}

DatagramServiceThread::~DatagramServiceThread() {
}

void DatagramServiceThread::start(int p, int mconn) {
	port = p;

	clients = new ServiceClientMap(mconn);

	if (serviceHandler == nullptr)
		throw ServiceException("no service handler assigned");

	try {
		SocketAddress addr(port);
		socket = new UDPServerSocket(&addr);

		socket->setBlocking(false);
	} catch (const SocketException& e) {
		socket = nullptr;

		error() << "unable to connect to socket on port " << port;

		throw ServiceException(e.getMessage());
	}

	serviceHandler->initialize();

	ServiceThread::start();

	info() << "started on port " << port;
}

void DatagramServiceThread::stop() {
	if (socket != nullptr) {
		ServiceThread::stop();

		socket->close();

		removeConnections();

		info("stopped");
	}
}

void DatagramServiceThread::run() {
	receiveMessages();
}

class MessageReceiverTask : public Task {
	DatagramServiceThread* service;

	Packet* message;
	SocketAddress address;

public:
	MessageReceiverTask(DatagramServiceThread* serv, Packet* packet, const SocketAddress& addr) {
		service = serv;

		message = packet->clone();
		address = addr;
	}

	~MessageReceiverTask() {
		delete message;
	}

	void run() {
		message->reset();

		service->receiveMessage(message, address);
	}
};

class DatagramMessageHandler : public IOHandler {
	DatagramServiceThread* service;

public:
	DatagramMessageHandler(DatagramServiceThread* service) : service(service) {
	}

	void handleInput(FileDescriptor* descriptor) {
		Socket* socket = (Socket*) descriptor;

		SocketAddress addr;
		Packet packet;

		if (socket->readFrom(&packet, &addr) != 0) {
			service->processMessage(&packet, addr);
		}
	}

	void handleOutput(FileDescriptor* descriptor) {
	}

	void handleHangup(FileDescriptor* descriptor) {
	}

	void handleError(FileDescriptor* descriptor) {
	}

};

void DatagramServiceThread::receiveMessages() {
	IOProcessor processor;
	DatagramMessageHandler handler(this);

	socket->setHandler(&handler);

	try {
		processor.initialize(1);
		processor.addFileDescriptor(socket/*, true*/);
	} catch (const IOException& e) {
		e.printStackTrace();
		throw ServiceException(e.getMessage());
	}

	while (doRun) {
		try	{
			processor.pollEvents(1000);
		} catch (SocketException& e) {
			debug(e.getMessage());
		} catch (Exception& e) {
			error(e.getMessage());
			e.printStackTrace();
		} catch (...) {
			fatal("unreported Exception caught");
		}
	}
}

void DatagramServiceThread::processMessage(Packet* packet, const SocketAddress& addr) {
	Reference<Task*> receiverTask = new MessageReceiverTask(this, packet, addr);

	receiverTask->executeInThread();
}

void DatagramServiceThread::receiveMessage(Packet* packet, SocketAddress& addr) {
	Reference<ServiceClient*> client = nullptr;

	try {
		uint64 netid = addr.getNetworkID();

		Locker locker(this);

		client = clients->get(netid);

		if (client == nullptr)	{
			if ((client = serviceHandler->createConnection(socket, addr)) == nullptr)
				return;

			clients->add(client);
		}

		locker.release();

		if (client->isAvailable()) {
			//serviceFilter->messageReceived(client, &packet);

			serviceHandler->handleMessage(client, packet);
		}
	} catch (SocketException& e) {
		if (!serviceHandler->handleError(client, e))
			setRunning(false);
	}
}
