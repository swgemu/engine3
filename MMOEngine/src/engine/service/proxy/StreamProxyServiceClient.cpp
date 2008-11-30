/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "StreamProxyService.h"

#include "StreamProxyClient.h"

#include "StreamProxyServiceClient.h"

StreamProxyServiceClient::StreamProxyServiceClient(StreamProxyService* serv, String& host, int port) 
	: StreamServiceClient(host, port), Logger()
{
	service = serv;

	setLogging(false);
}

void StreamProxyServiceClient::run()
{
	info("started");

	receiveMessages();

	info("stopped");
}

void StreamProxyServiceClient::handleMessage(Packet* message)
{
	forwardMessage(message);
}

bool StreamProxyServiceClient::handleError(Exception& e) 
{
	stop();

	return false;
}

void StreamProxyServiceClient::forwardMessage(Packet* message) 
{
	if (message->size() == 0)
	{
		info("not forwarding zero-sized packet");
		return;
	}

	Packet* msg = message->clone();

	//info("forwarding message: " + message->toString(), true);
	proxyClient->send(msg);
}

void StreamProxyServiceClient::setProxyClient(StreamProxyClient* pclient)
{
	proxyClient = pclient;
	setLoggingName("StreamProxyServiceClient = " + pclient->getAddress().getFullIPAddress());
}
