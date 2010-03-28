/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMPROXYCLIENT_H_
#define DATAGRAMPROXYCLIENT_H_

#include "engine/engine.h"

class DatagramProxyServiceClient;

class DatagramProxyClient : public DatagramServiceClientProxy, public Logger {
	class DatagramProxyService* service;

	Reference<DatagramProxyServiceClient*> proxyServiceClient;
	
public:
	DatagramProxyClient(DatagramProxyService* serv, Socket* sock, SocketAddress& addr);

	virtual ~DatagramProxyClient();
	
	void forwardMessage(Packet* message);

	void closeClients();
	
	inline bool isAvailable() {
		return proxyServiceClient != NULL;
	}
	
	// setters
	inline void setProxyServiceClient(DatagramProxyServiceClient* pclient) {
		proxyServiceClient = pclient;
	}

};

#endif /*DATAGRAMPROXYCLIENT_H_*/
