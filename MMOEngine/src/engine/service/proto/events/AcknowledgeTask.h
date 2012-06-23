/*
 * AcknowledgeTask.h
 *
 *  Created on: 23/06/2012
 *      Author: victor
 */

#ifndef ACKNOWLEDGETASK_H_
#define ACKNOWLEDGETASK_H_

class AcknowledgeTask : public Task {
	Reference<BaseClient*> client;
	uint16 seq;

public:
	AcknowledgeTask(BaseClient* cl, uint16 sequence) {
		client = cl;
		seq = sequence;
	}

	void run() {
		client->acknowledgeServerPackets(seq);
	}
};


#endif /* ACKNOWLEDGETASK_H_ */
