/*
 * OutOfOrderTask.h
 *
 *  Created on: 23/06/2012
 *      Author: victor
 */

#ifndef OUTOFORDERTASK_H_
#define OUTOFORDERTASK_H_

class OutOfOrderTask : public Task {
	Reference<BaseClient*> client;
	uint16 seq;

public:
	OutOfOrderTask(BaseClient* cl, uint16 sequence) {
		client = cl;
		seq = sequence;
	}

	void run() {
		client->resendPackets(seq);
	}
};


#endif /* OUTOFORDERTASK_H_ */
