/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "StreamProxyClient.h"

#include "StreamProxyService.h"

StreamProxyService::StreamProxyService() : StreamServiceThread("StreamProxyService") {
}

void StreamProxyService::init() {
	setLogging(false);
	setGlobalLogging(true);
}

void StreamProxyService::run() {
	ServiceThread::run();

	acceptConnections();
}

void StreamProxyService::stop() {
	StreamServiceThread::stop();

	//proxyServiceClient->stop();
}

ServiceClient* StreamProxyService::createConnection(Socket* sock, SocketAddress& addr) {
	StreamProxyClient* pclient = new StreamProxyClient(this, sock, addr);

	proxyServiceClient = new StreamProxyServiceClient(this, forwardAddress, forwardPort);

	pclient->setProxyServiceClient(proxyServiceClient);
	proxyServiceClient->setProxyClient(pclient);

	proxyServiceClient->connect();
	proxyServiceClient->start();

	pclient->start();

	info("forwarding messages for \'" + addr.getFullIPAddress() + "\'");

	return pclient;
}

bool StreamProxyService::handleError(Exception& e) {
	return true;
}

void StreamProxyService::setForwarding(const String& address, int port) {
	forwardAddress = address;
	forwardPort = port;

	StringBuffer msg;
	msg << "registered proxy \'" << address << ":" << port << "\'";
	info(msg);
}

void StreamProxyService::setName(const String& name, int port) {
	StringBuffer logname;
	logname << name << " = " << port;

	setLoggingName(logname.toString());
}

String& StreamProxyService::getAddress() {
	if (fullAddress.isEmpty()) {
		StringBuffer addr;
		addr << forwardAddress << ":" << forwardPort;

		fullAddress = addr.toString();
	}

	return fullAddress;
}
