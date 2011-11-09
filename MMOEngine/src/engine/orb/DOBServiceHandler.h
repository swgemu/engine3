/*
 * DistributedObjectBrokerServiceHandler.h
 *
 *  Created on: 03/07/2011
 *      Author: victor
 */

#ifndef DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_
#define DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_

#include "DistributedObjectBroker.h"

class DOBServiceHandler : public ServiceHandler {
	DistributedObjectBroker* broker;
public:
	DOBServiceHandler(DistributedObjectBroker* broker) {
		this->broker = broker;
	}

	void initialize() {

	}

	ServiceClient* createConnection(Socket* sock, SocketAddress& addr) {
		return broker->createConnection(sock, addr);
	}

	bool deleteConnection(ServiceClient* client) {
		//return broker->deleteConnection(client);
		return false;
	}

	void handleMessage(ServiceClient* client, Packet* message) {
		broker->getPacketHandler()->handlePacket((DistributedObjectBrokerClient*)client, message);
	}

	void processMessage(Message* message) {
		//broker->processMessage(message);
	}

	bool handleError(ServiceClient* client, Exception& e) {
		//return broker->handleError(client, e);
		return false;
	}
};



#endif /* DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_ */
