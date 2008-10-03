/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMPROXYSERVICE_H_
#define DATAGRAMPROXYSERVICE_H_

#include "engine/engine.h"

class DatagramProxyService : public DatagramServiceThread {
	string forwardAddress;
	int forwardPort;

	string fullAddress;

public:
	DatagramProxyService();

	void init();

	void run();

	void stop();

	// client methods
	ServiceClient* createConnection(Socket* sock, SocketAddress& addr);

	void handleMessage(ServiceClient* client, Packet* message);

	bool handleError(Exception& e);

	void setForwarding(const string& address, int port);

	// setters and getters
	void setName(const string& name, int port);

	string& getAddress();

};

#endif /*DATAGRAMPROXYSERVICE_H_*/
