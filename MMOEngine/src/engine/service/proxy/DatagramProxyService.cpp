/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DatagramProxyServiceClient.h"
#include "DatagramProxyClient.h"

#include "DatagramProxyService.h"

DatagramProxyService::DatagramProxyService() : DatagramServiceThread("DatagramProxyService") {
}

void DatagramProxyService::init() {
	setLogging(false);
	setGlobalLogging(true);
}

void DatagramProxyService::run() {
	ServiceThread::run();

	receiveMessages();
}

void DatagramProxyService::stop() {
	DatagramServiceThread::stop();
}

ServiceClient* DatagramProxyService::createConnection(Socket* sock, SocketAddress& addr) {
	DatagramProxyClient* pclient = new DatagramProxyClient(this, socket, addr);

	DatagramProxyServiceClient* proxyServiceClient = new DatagramProxyServiceClient(this, forwardAddress, forwardPort);
	proxyServiceClient->start();

	pclient->setProxyServiceClient(proxyServiceClient);
	proxyServiceClient->setProxyClient(pclient);

	info("forwarding messages for \'" + addr.getFullIPAddress() + "\'");

	return pclient;
}

void DatagramProxyService::handleMessage(ServiceClient* client, Packet* message) {
	DatagramProxyClient* pclient = (DatagramProxyClient*) client;
	pclient->forwardMessage(message);

	/*if (!pclient->isAvailable())
		delete pclient;*/
}

bool DatagramProxyService::handleError(Exception& e) {
	return true;
}

void DatagramProxyService::setForwarding(const String& address, int port) {
	forwardAddress = address;
	forwardPort = port;

	StringBuffer msg;
	msg << "registered proxy \'" << address << ":" << port << "\'";
	info(msg);
}

void DatagramProxyService::setName(const String& name, int port) {
	StringBuffer logname;
	logname << name << " = " << port;

	setLoggingName(logname.toString());
}

String& DatagramProxyService::getAddress() {
	if (fullAddress.isEmpty()) {
		StringBuffer addr;
		addr << forwardAddress << ":" << forwardPort;

		fullAddress = addr.toString();
	}

	return fullAddress;
}
