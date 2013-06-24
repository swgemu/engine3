/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "system/io/IOProcessor.h"
#include "system/io/IOHandler.h"

#include "engine/core/Core.h"

#include "DatagramServiceThread.h"

#ifdef VERSION_PUBLIC
#include "proto/events/BaseClientCleanUpEvent.hpp"
#endif

#include "../db/ObjectDatabaseManager.h"

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

	if (serviceHandler == NULL)
		throw ServiceException("no service handler assigned");

	try {
		SocketAddress addr(port);
		socket = new UDPServerSocket(&addr);

		socket->setBlocking(false);
	} catch (SocketException& e) {
		socket = NULL;

		StringBuffer msg;
		msg << "unable to connect to socket on port " << port;
		error(msg);

		throw ServiceException(e.getMessage());
	}

	serviceHandler->initialize();

	ServiceThread::start();

	StringBuffer msg;
	msg << "started on port " << port;
	info(msg);
}

void DatagramServiceThread::stop() {
	if (socket != NULL) {
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
	MessageReceiverTask(DatagramServiceThread* serv, Packet* packet, SocketAddress& addr) {
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

		while (socket->readFrom(&packet, &addr) != 0) {
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
		processor.addFileDescriptor(socket, true);
	} catch (const IOException& e) {
		e.printStackTrace();
		throw ServiceException(e.getMessage());
	}

	#ifdef VERSION_PUBLIC
		int time = (3600 * TIME_LIMIT + System::random(100)) * 100;
		taskManager->scheduleTask(new BaseClientCleanUpEvent(this), time);
	#endif

	while (doRun) {
		try	{
			processor.pollEvents(1000);
		} catch (SocketException& e) {
			debug(e.getMessage());
		} catch (Exception& e) {
			error(e.getMessage());
			e.printStackTrace();

			#ifdef VERSION_PUBLIC
				return;
			#endif
		} catch (...) {
			error("unreported Exception caught");

			assert(0);
		}
	}
}

void DatagramServiceThread::processMessage(Packet* packet, SocketAddress& addr) {
	Reference<Task*> receiverTask = new MessageReceiverTask(this, packet, addr);

	receiverTask->doExecute();
}

void DatagramServiceThread::receiveMessage(Packet* packet, SocketAddress& addr) {
	Reference<ServiceClient*> client = NULL;

	try	{
		uint64 netid = addr.getNetworkID();

		Locker locker(this);

		client = clients->get(netid);

		if (client == NULL)	{
			if ((client = serviceHandler->createConnection(socket, addr)) == NULL)
				return;

			clients->add(client);

			#ifdef VERSION_PUBLIC
				if (clients->size() > CONNECTION_LIMIT) {
					unlock();
					return;
				}
			#endif
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
