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

#if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
		setStatsSample(0);
#endif
	}

	void run() {
		client->setClientDisconnected();
		client->disconnect();
	}
};


#endif /* DISCONNECTTASK_H_ */
