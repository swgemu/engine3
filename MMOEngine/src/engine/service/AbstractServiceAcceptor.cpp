/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "AbstractServiceAcceptor.h"

AbstractServiceAcceptor::AbstractServiceAcceptor(const String& s) : ServiceThread(s) {
	sessions = NULL;

	session = NULL;

	handler = new FilteringServiceHandler();
}

AbstractServiceAcceptor::~AbstractServiceAcceptor() {
	if (sessions != NULL) {
		delete sessions;
		sessions = NULL;
	}

	if (session != NULL) {
		delete session;
		session = NULL;
	}
}

bool AbstractServiceAcceptor::deleteConnection(ServiceSession* client) {
	lock();
	
	bool res = sessions->remove(client);
	
	unlock();
	return res;
}
