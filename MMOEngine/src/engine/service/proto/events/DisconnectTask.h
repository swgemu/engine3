/*
 * DisconnectTask.h
 *
 *  Created on: 23/06/2012
 *      Author: victor
 */

#ifndef DISCONNECTTASK_H_
#define DISCONNECTTASK_H_

class DisconnectTask : public Task {
	Reference<BaseClient*> client;
public:
	DisconnectTask(BaseClient* cl) {
		client = cl;

#ifdef BASECLIENT_DISABLE_STATSD
		setStatsSample(0);
#endif
	}

	void run() {
		client->setClientDisconnected();
		client->disconnect();
	}
};


#endif /* DISCONNECTTASK_H_ */
