/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
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

#if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
		setStatsSample(0);
#endif
	}

	void run() {
		client->resendPackets(seq);
	}
};


#endif /* OUTOFORDERTASK_H_ */
