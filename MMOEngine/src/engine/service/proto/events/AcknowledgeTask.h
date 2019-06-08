/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
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

#if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
		setStatsSample(0);
#endif
	}

	void run() {
		client->acknowledgeServerPackets(seq);
	}
};


#endif /* ACKNOWLEDGETASK_H_ */
