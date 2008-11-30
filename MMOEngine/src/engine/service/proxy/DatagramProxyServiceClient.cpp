/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DatagramProxyClient.h"
#include "DatagramProxyService.h"

#include "DatagramProxyServiceClient.h"

#include "../../sched/ScheduleManager.h"

DatagramProxyServiceClient::DatagramProxyServiceClient(DatagramProxyService* serv, String& host, int port) 
		: DatagramServiceClient(host, port), Thread(), Logger("DatagramProxyServiceClient") {
	service = serv;

	doRun = true;

	//setDetached();

	setLoggingName("DatagramProxyServiceClient " + addr.getFullIPAddress());
	setLogging(true);
}

DatagramProxyServiceClient::~DatagramProxyServiceClient() {
}

void DatagramProxyServiceClient::run() {
	info("started");

	Packet packet;

	while (doRun) {
		try	{
			if (read(&packet))
				handleMessage(&packet);
		} catch (SocketException&) {
			doRun = false;
		}
	}

	close();

	info("stopped");
	
	finalize();
}

void DatagramProxyServiceClient::stop() {
	if (doRun) {
		info("stopping");
		
		doRun = false;
	
		join();
	}
}

void DatagramProxyServiceClient::handleMessage(Packet* message) {
	forwardMessage(message);
}

void DatagramProxyServiceClient::forwardMessage(Packet* message) {
	/*if (message->size() == 0) {
		info("not forwarding zero-sized packet");
		return;
	}*/

	//info("forwarding message:  " + message->toString());

	Packet* msg = message->clone();
	proxyClient->send(msg);
}

