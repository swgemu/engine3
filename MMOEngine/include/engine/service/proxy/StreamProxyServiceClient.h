/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STREAMPROXYSERVICECLIENT_H_
#define STREAMPROXYSERVICECLIENT_H_

#include "engine/engine.h"

class StreamProxyServiceClient : public StreamServiceClient, public Logger 
{
	class StreamProxyService* service;

	class StreamProxyClient* proxyClient;

public:
	StreamProxyServiceClient(StreamProxyService* serv, String& host, int port);

	virtual ~StreamProxyServiceClient() {
	}

	void run();

	void handleMessage(Packet* message);

	bool handleError(Exception& e);

	void forwardMessage(Packet* message);

	// setters
	void setProxyClient(StreamProxyClient* pclient);

};

#endif /*STREAMPROXYSERVICECLIENT_H_*/
