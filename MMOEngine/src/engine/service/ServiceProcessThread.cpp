/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ServiceProcessThread.h"
#include "../db/ObjectDatabaseManager.h"

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
	ObjectDatabaseManager::instance()->commitLocalTransaction();

	Message* msg;

	while ((msg = server->getMessage()) != NULL) {
		ObjectDatabaseManager::instance()->startLocalTransaction();

		try {
			processMessage(msg);
		} catch (PacketIndexOutOfBoundsException& e) {
			ObjectDatabaseManager::instance()->commitLocalTransaction();

			if (!handleError(msg, e))
				break;
		} catch (...) {
			error("unreported Exception caught");
			info(msg->toStringData(), true);
		}

		delete msg;

		ObjectDatabaseManager::instance()->commitLocalTransaction();
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
