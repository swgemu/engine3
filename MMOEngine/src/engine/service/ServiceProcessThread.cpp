/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ServiceProcessThread.h"

ServiceProcessThread::ServiceProcessThread(const String& s) : ServiceThread(s) {
}

ServiceProcessThread::~ServiceProcessThread() {
}

void ServiceProcessThread::start(ServiceMessageHandlerThread* serv) {
	server = serv;

	info("started");

	ServiceThread::start(false);
}

void ServiceProcessThread::run() {
	Message* msg;

	while ((msg = server->getMessage()) != NULL) {
		try {
			processMessage(msg);
		} catch (PacketIndexOutOfBoundsException& e) {
			if (!handleError(msg, e))
				break;
		} catch (...) {
			error("unreported Exception caught");
			info(msg->toStringData(), true);
		}

		delete msg;
	}

	server->flushMessages();
}

void ServiceProcessThread::stop() {
	ServiceThread::stop();

	info("stopped");
}

void ServiceProcessThread::processMessage(Message* message) {
}

bool ServiceProcessThread::handleError(Message* msg, Exception& e) {
	return false;
}
