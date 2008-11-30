/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "StreamServiceClientProxy.h"

#include "StreamServiceThread.h"

StreamServiceThread::StreamServiceThread(const String& s) : ServiceMessageHandlerThread(s) {
	setLogging(false);
}

StreamServiceThread::~StreamServiceThread() {
}

void StreamServiceThread::start(int p, int mconn) {
	port = p;

	clients = new ServiceClientMap(mconn); 
			
	try {
		SocketAddress addr(port); 
		socket = new TCPServerSocket(&addr);
			
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

void StreamServiceThread::stop() {
	if (socket != NULL) {
		socket->close();

		ServiceThread::stop();
		
		info("stopped", true);
	}
}

void StreamServiceThread::acceptConnections(int limit) {
	try {
		socket->listen(limit);

		while (doRun) {
			SocketAddress address;

			Socket* sock = socket->accept(&address);
			sock->disableLingering();
			
			createConnection(sock, address);
		}	
	} catch (...) {
	}
}

bool StreamServiceThread::closeConnection(ServiceClient* client) {
	client->finalize();
	
	return true;
}
