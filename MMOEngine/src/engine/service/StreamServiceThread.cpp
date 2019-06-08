/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "StreamServiceClientProxy.h"

#include "StreamServiceThread.h"

StreamServiceThread::StreamServiceThread(const String& s) : ServiceMessageHandlerThread(s) {
	setInfoLogLevel();
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
		socket = nullptr;

		StringBuffer msg;
		msg << "unable to connect to socket on port " << port;
		error(msg);

		throw ServiceException(e.getMessage());
	}

	init();

	ServiceThread::start();

	StringBuffer msg;
	msg << "started on port " << port;
	info(msg);
}

void StreamServiceThread::stop() {
	if (socket != nullptr) {
		setRunning(false);

		socket->shutdown(SHUT_RDWR);
		socket->close();

		ServiceThread::stop(true);
		
		info("stopped");
	}
}

void StreamServiceThread::acceptConnections(int limit) {
	try {
		socket->listen(limit);

		while (doRun) {
			SocketAddress address;

			Socket* sock = socket->accept(&address);
			sock->disableLingering();
			
			assert(serviceHandler != nullptr);
			serviceHandler->createConnection(sock, address);
		}	
	} catch (...) {
	}
}

bool StreamServiceThread::closeConnection(ServiceClient* client) {
	client->finalize();
	
	return true;
}
