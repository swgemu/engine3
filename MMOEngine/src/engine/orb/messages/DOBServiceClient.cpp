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

	//setLogging(true);
	//setGlobalLogging(true);

	info("client connected");
}

DOBServiceClient::DOBServiceClient(const String& host, int port)
		: StreamServiceClient(host, port) , Logger("DistributedObjectBroker") {
	rob = NULL;

	serviceHandler = new DOBServiceHandler();

	//setLogging(true);
	//setGlobalLogging(true);

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

void DOBServiceClient::receiveMessages() {
	Packet packet;

	assert(serviceHandler);

	int messageSize = 0;

	while (doRun) {
		try	{
			if (recieve(&packet)) {
				//printf("ran recieve with data:%s\n", packet.toStringData().toCharArray());

				if (packet.size() == 0)
					break;

				while (packet.size() > 0) {
					//printf("ran while loop with data:%s\n", packet.toStringData().toCharArray());

					if (messageSize && (messageSize <= packet.size())) {
						//printf("messageSize:%d packet.size() pre handle:%d\n", messageSize, packet.size());
						packet.reset();

						serviceHandler->handleMessage(this, &packet);

						//printf("messageSize:%d packet.size() post handle:%d\n", messageSize, packet.size());
						packet.removeRange(0, messageSize);
						messageSize = 0;
						//printf("messageSize:%d packet.size() post remove range:%d\n", messageSize, packet.size());
					} else {
						if (!messageSize) {
							if (packet.size() >= 4) {
								packet.reset();

								messageSize = packet.parseInt() - 4;

								packet.removeRange(0, 4);
							} else {
								break;
							}
						} else {
							break;
						}
					}
				}
			}
		} catch (SocketException& e) {
			if (!serviceHandler->handleError(this, e))
				break;
		}
	}

	disconnect();
}


bool DOBServiceClient::send(DOBMessage* message) {
	message->setSequence(sentMessageSequence.increment());
	message->setSize();

	info("SEND " + message->toStringData());
	StreamServiceClient::send(message);

	//delete message;
	return true;
}


bool DOBServiceClient::sendReply(DOBMessage* message) {
	message->setSize();

	info("SEND " + message->toStringData());

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
	message->setSize();

	sentMessageQueue.put(sequence, message);

	info("SEND " + message->toStringData());
	return StreamServiceClient::send(message);
}

bool DOBServiceClient::waitForReply(DOBMessage* message) {
	bool res = message->waitForReply();

	if (!res)
		error("timeout on message " + String::valueOf(message->getSequence()));

	return res;
}
