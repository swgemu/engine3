/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "StreamProxyService.h"

#include "StreamProxyServiceClient.h"

#include "StreamProxyClient.h"

StreamProxyClient::StreamProxyClient(StreamProxyService* serv, Socket* sock, SocketAddress& addr) 
	: StreamServiceClientProxy(sock, addr), Logger()
{
	service = serv;

	setDetached();
	
	setLoggingName("StreamProxyClient = " + addr.getFullIPAddress());
	setLogging(false);
}

void StreamProxyClient::run()
{
	info("started");

	receiveMessages();

	proxyClient->stop();

	info("stopped");
}

void StreamProxyClient::handleMessage(Packet* message)
{
	forwardMessage(message);
}

bool StreamProxyClient::handleError(Exception& e) {
	proxyClient->stop();

	stop();

	return false;
}

void StreamProxyClient::forwardMessage(Packet* message) 
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
