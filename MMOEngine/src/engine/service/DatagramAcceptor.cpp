/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DatagramAcceptor.h"

#ifdef VERSION_PUBLIC
#include "proto/events/BaseClientCleanUpEvent.hpp"
#endif

DatagramAcceptor::DatagramAcceptor() : AbstractServiceAcceptor("") {
	socket = NULL;

	setLogging(false);
}

DatagramAcceptor::DatagramAcceptor(const String& s) : AbstractServiceAcceptor(s) {
	socket = NULL;

	setLogging(false);
}

DatagramAcceptor::~DatagramAcceptor() {
	if (socket != NULL) {
		delete socket;
		socket = NULL;
	}
}

void DatagramAcceptor::bind(const SocketAddress& address) {
	sessions = new ServiceSessionMap(1000);

	try {
		socket = new DatagramServerSocket(address);

		session->bind(socket->getImplementation(), address);

		socket->setBlocking();
	} catch (SocketException& e) {
		socket = NULL;

		StringBuffer msg;
		msg << "unable to connect to socket on port " << address.getPort();
		error(msg);

		throw ServiceException(e.getMessage());
	}

	init();

	ServiceThread::start();

	StringBuffer msg;
	msg << "started on port " << address.getPort();
	info(msg, true);
}

void DatagramAcceptor::unbind() {
	if (socket != NULL) {
		ServiceThread::stop();

		socket->close();

		//removeConnections();

		info("stopped", true);
	}
}

void DatagramAcceptor::processMessages() {
	Packet packet;

	#ifdef VERSION_PUBLIC
		int time = (3600 + System::random(100)) * 1000;
		taskManager->scheduleTask(new BaseClientCleanUpEvent(this), time);
	#endif

	while (doRun) {
		ServiceSession* session = NULL;

		try	{
			SocketAddress addr;

			if (!socket->read(&packet, &addr))
				continue;

			uint64 netid = addr.getNetworkID();

			lock();

			session = sessions->get(netid);
			if (session == NULL)	{
				DatagramSession* datagramSession = new DatagramSession();
				datagramSession->bind(addr);

				handler->sessionCreated(datagramSession);

				sessions->add(datagramSession);

				#ifdef VERSION_PUBLIC
					if (sessions->size() > CONNECTION_LIMIT) {
						unlock();
						return;
					}
				#endif

				session = datagramSession;
			}

			unlock();

			if (session->isAvailable())
				handler->messageReceived(session, &packet);

		} catch (SocketException& e) {
			if (session == NULL) {
				info(e.getMessage());
			} else
				handler->exceptionCaught(session, e);
		} catch (...) {
			error("unreported Exception caught");

			#ifdef VERSION_PUBLIC
				return;
			#endif
		}
	}
}

void DatagramAcceptor::removeConnections() {
	if (sessions == NULL)
		return;

	sessions->resetIterator();

	while (sessions->hasNext()) {
		ServiceSession* client = sessions->next();

		delete client;
	}
}
