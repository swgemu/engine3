/*
 * DistributedObjectBrokerServiceHandler.h
 *
 *  Created on: 03/07/2011
 *      Author: victor
 */

#ifndef DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_
#define DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_

#include "engine/orb/DistributedObjectBroker.h"

#include "DOBMessageFactory.h"

class DOBServiceHandler : public ServiceHandler {
	DOBMessageFactory messageFactory;

public:
	DOBServiceHandler() {
	}

	void initialize() {

	}

	ServiceClient* createConnection(Socket* sock, SocketAddress& addr) {
		DOBServiceClient* client = new DOBServiceClient(sock);
		client->setHandler(this);
		client->start();

		return client;
	}

	bool deleteConnection(ServiceClient* client) {
		//client->stop();
		return true;
	}

	void handleMessage(ServiceClient* client, Packet* message) {
		messageFactory.process(static_cast<DOBServiceClient*>(client), message);
	}

	void processMessage(Message* message) {
		//broker->processMessage(message);
	}

	void messageSent(Packet* message){
		uint32 messageType = message->parseInt(0);

		//System::out << "DOBMessage: " << messageType << " sent with content: " << message->toStringData() << "\n";
	}

	bool handleError(ServiceClient* client, Exception& e) {
		//return broker->handleError(client, e);
		return false;
	}
};



#endif /* DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_ */
