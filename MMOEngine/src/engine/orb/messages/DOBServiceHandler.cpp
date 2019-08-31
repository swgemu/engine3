/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 17/10/16.
//

#include "DOBServiceHandler.h"

DOBServiceHandler::DOBServiceHandler() {
}

void DOBServiceHandler::initialize() {

}

ServiceClient* DOBServiceHandler::createConnection(Socket* sock, SocketAddress& addr) {
	DOBServiceClient* client = new DOBServiceClient(sock);
	client->setHandler(this);
	client->start();

	RemoteObjectBroker* broker = client->getRemoteObjectBroker();

	ObjectBrokerDirector::instance()->brokerConnected(broker);

	return client;
}

bool DOBServiceHandler::deleteConnection(ServiceClient* client) {
	DOBServiceClient* dobClient = static_cast<DOBServiceClient*>(client);
	RemoteObjectBroker* broker = dobClient->getRemoteObjectBroker();

	ObjectBrokerDirector::instance()->brokerDisconnected(broker);

	dobClient->stop();

	return true;
}

void DOBServiceHandler::handleMessage(ServiceClient* client, Packet* message) {
	DOBServiceClient* dobClient = static_cast<DOBServiceClient*>(client);

	messageFactory.process(dobClient, message);
}

void DOBServiceHandler::processMessage(Message* message) {
	//broker->processMessage(message);
}

void DOBServiceHandler::messageSent(Packet* packet){
	DOBMessage* message = static_cast<DOBMessage*>(packet);

	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	uint32 messageType = message->parseInt(0);

	broker->debug() << "DOBMessage(" << message->getSequence() << "): " << messageType
				  << " sent with content: " << message->toStringData();
}

bool DOBServiceHandler::handleError(ServiceClient* client, Exception& e) {
	//return broker->handleError(client, e);
	return false;
}
