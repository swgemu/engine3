/*
 * DistributedObjectBrokerServiceHandler.h
 *
 *  Created on: 03/07/2011
 *      Author: victor
 */

#ifndef DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_
#define DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/messages/RemoteObjectBroker.h"

#include "engine/orb/control/ObjectBrokerDirector.h"

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

		RemoteObjectBroker* broker = client->getRemoteObjectBroker();

		ObjectBrokerDirector::instance()->brokerConnected(broker);

		return client;
	}

	bool deleteConnection(ServiceClient* client) {
		DOBServiceClient* dobClient = static_cast<DOBServiceClient*>(client);
		dobClient->stop();

		ObjectBroker* broker = dobClient->getRemoteObjectBroker();

		ObjectBrokerDirector::instance()->brokerDisconnected(broker);

		return true;
	}

	void handleMessage(ServiceClient* client, Packet* message) {
		DOBServiceClient* dobClient = static_cast<DOBServiceClient*>(client);

		while (message->hasData())
			messageFactory.process(dobClient, message);
	}

	void processMessage(Message* message) {
		//broker->processMessage(message);
	}

	void messageSent(Packet* packet){
		DOBMessage* message = static_cast<DOBMessage*>(packet);

		DistributedObjectBroker* broker = DistributedObjectBroker::instance();

		uint32 messageType = message->parseInt(0);

		broker->debug("DOBMessage(" + String::valueOf(message->getSequence()) + "): " + String::valueOf(messageType)
						+ " sent with content: " + message->toStringData());
	}

	bool handleError(ServiceClient* client, Exception& e) {
		//return broker->handleError(client, e);
		return false;
	}
};



#endif /* DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_ */
