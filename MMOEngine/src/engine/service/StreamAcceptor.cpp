/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "StreamSession.h"

#include "StreamAcceptor.h"

StreamAcceptor::StreamAcceptor(const String& s) : AbstractServiceAcceptor(s) {
	socket = NULL;

	setLogging(false);
}

StreamAcceptor::~StreamAcceptor() {
	if (socket != NULL) {
		delete socket;
		socket = NULL;
	}
}

void StreamAcceptor::start(int p, int mconn) {
	port = p;

	sessions = new ServiceSessionMap(mconn); 
			
	try {
		SocketAddress addr(port); 
		socket = new StreamServerSocket(addr);
			
		socket->disableLingering();
		socket->setBlocking(true);
	} catch (SocketException& e) {
		socket = NULL;

		StringBuffer msg;
		msg << "unable to connect to socket on port " << port;
		error(msg);

		throw ServiceException(e.getMessage());
	}

	init();

	ServiceThread::start();

	StringBuffer msg;
	msg << "started on port " << port;
	info(msg, true); 
}

void StreamAcceptor::stop() {
	if (socket != NULL) {
		socket->close();

		ServiceThread::stop();

		info("stopped", true);
	}
}

void StreamAcceptor::acceptConnections(int limit) {
	try {
		socket->listen(limit);

		while (doRun) {
			SocketAddress address;

			SocketImplementation* sock = socket->accept(address);
			sock->disableLingering();
			
			session->bind(sock, address);

			handler->sessionCreated(session);
		}	
	} catch (...) {
	}
}

bool StreamAcceptor::closeConnection(ServiceSession* client) {
	client->finalize();
	
	return true;
}
