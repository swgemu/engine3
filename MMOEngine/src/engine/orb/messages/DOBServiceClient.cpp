/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/orb/DistributedObjectBroker.h"

#include "RemoteObjectBroker.h"

#include "DOBServiceHandler.h"

DOBServiceClient::DOBServiceClient(Socket* sock)
		: StreamServiceClient(sock), Logger("DistributedObjectBroker") {
	/*info("client \'" + host + "\' connected", true);*/

	rob = new RemoteObjectBroker(this);

	serviceHandler = NULL;

	info("client connected");
}

DOBServiceClient::DOBServiceClient(const String& host, int port)
		: StreamServiceClient(host, port) , Logger("DistributedObjectBroker") {
	rob = NULL;

	serviceHandler = new DOBServiceHandler();

	try {
		connect();
		info("connected to server \'" + host + "\'");
	} catch (SocketException& e) {
		error("unable to connect to \'" + host + "\'");
		error(e.getMessage());
	}
}

DOBServiceClient::~DOBServiceClient() {
	if (rob != NULL) {
		delete rob;
		rob = NULL;
	}
}

void DOBServiceClient::run() {
	receiveMessages();
}

bool DOBServiceClient::send(DOBMessage* message) {
	message->setSequence(sentMessageSequence.increment());

	//info("SEND " + pack->toString());
	StreamServiceClient::send(message);
	
	//delete message;
	return true;
}


bool DOBServiceClient::sendReply(DOBMessage* message) {
	//info("SEND " + pack->toString());
	StreamServiceClient::send(message);

	//delete message;
	return true;
}

bool DOBServiceClient::sendAndAcceptReply(DOBMessage* message) {
	sendWithReply(message);

	return waitForReply(message);
}

bool DOBServiceClient::sendWithReply(DOBMessage* message) {
	uint32 sequence = sentMessageSequence.increment();
	message->setSequence(sequence);

	sentMessageQueue.put(sequence, message);

	//info("SEND " + pack->toString());
	return StreamServiceClient::send(message);
}

bool DOBServiceClient::waitForReply(DOBMessage* message) {
	bool res = message->waitForReply();

	if (!res)
		error("timeout on message " + String::valueOf(message->getSequence()));

	return res;
}
