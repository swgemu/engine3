/*
 * SessionStartTask.h
 *
 *  Created on: 23/06/2012
 *      Author: victor
 */

#ifndef SESSIONSTARTTASK_H_
#define SESSIONSTARTTASK_H_

#include "engine/service/proto/BaseClient.h"

class SessionStartTask : public Task {
	Reference<BaseClient*> client;
	uint32 connectionID;

public:
	SessionStartTask(BaseClient* cl, uint32 cid) {
		client = cl;
		connectionID = cid;
	}

	void run() {
		client->setConnectionID(connectionID);

		Packet* msg = new SessionIDResponseMessage(client);
		client->send(msg);
	}
};


#endif /* SESSIONSTARTTASK_H_ */
