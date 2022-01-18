/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * NetStatusResponseTask.h
 *
 *  Created on: 23/06/2012
 *      Author: victor
 */

#ifndef NETSTATUSRESPONSETASK_H_
#define NETSTATUSRESPONSETASK_H_

#include "engine/log/Logger.h"

class NetStatusResponseTask : public Task {
	Reference<BaseClient*> client;
	Packet* pack;
public:

	NetStatusResponseTask(BaseClient* cl, Packet* inPack) {
		client = cl;
		pack = inPack->clone();

#if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
		setStatsSample(0);
#endif
	}

	~NetStatusResponseTask() {
		if (pack != nullptr) {
			delete pack;
		}
	}

	void run() {
		client->handleNetStatusRequest(pack);
		delete pack;
		pack = nullptr;
	}
};


#endif /* NETSTATUSRESPONSETASK_H_ */
