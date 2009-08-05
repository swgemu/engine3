/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DatagramProxyServiceClient.h"
#include "DatagramProxyService.h"

#include "DatagramProxyClient.h"

DatagramProxyClient::DatagramProxyClient(DatagramProxyService* serv, Socket* sock, SocketAddress& addr) 
	: DatagramServiceClientProxy(sock, addr), Logger("DatagramProxyClient") {
	service = serv;

	setLogging(true);	
}

DatagramProxyClient::~DatagramProxyClient() {
	closeClients();	
}

void DatagramProxyClient::forwardMessage(Packet* message) {
	TaskManager* taskManager = TaskManager::instance();

	//info("forwarding message:  " + message->toString());	inline void updateSendStamp() {

	Packet* msg = message->clone();
	proxyServiceClient->send(msg);

	uint16 opcode;
	
	if (message->size() > 6)
		opcode = message->parseShort(5);
	else
		return;

	if (opcode == 0x01A0 || opcode == 0x4000 || opcode == 0x4180) {
		closeClients();
		
		info("closed");
		
		finalize();
	}
}

void DatagramProxyClient::closeClients() {
	if (proxyServiceClient != NULL) {
		proxyServiceClient->stop();

		service->removeConnection(this);

		proxyServiceClient = NULL;
	}
}
