/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
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
	DOBServiceHandler();

	void initialize();

	ServiceClient* createConnection(Socket* sock, SocketAddress& addr);

	bool deleteConnection(ServiceClient* client);

	void handleMessage(ServiceClient* client, Packet* message);

	void processMessage(Message* message);

	void messageSent(Packet* packet);

	bool handleError(ServiceClient* client, Exception& e);
};



#endif /* DISTRIBUTEDOBJECTBROKERSERVICEHANDLER_H_ */
