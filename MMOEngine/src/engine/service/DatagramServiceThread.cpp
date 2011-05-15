/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

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

		socket->setBlocking(true);
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

void DatagramServiceThread::receiveMessages() {
	Packet packet;

	#ifdef VERSION_PUBLIC
		int time = (3600 + System::random(100)) * 1000;
		taskManager->scheduleTask(new BaseClientCleanUpEvent(this), time);
	#endif

	while (doRun) {
		try	{
			SocketAddress addr;

			if (!socket->recieveFrom(&packet, &addr))
				continue;

			Reference<Task*> receiverTask = new MessageReceiverTask(this, &packet, addr);
			receiverTask->execute();
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
