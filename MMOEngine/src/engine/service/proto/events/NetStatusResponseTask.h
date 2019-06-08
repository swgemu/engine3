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

#include "engine/service/proto/packets/NetStatusResponseMessage.h"

class NetStatusResponseTask : public Task {
	Reference<BaseClient*> client;
	uint16 tick;
public:

	NetStatusResponseTask(BaseClient* cl, uint16 ti) {
		client = cl;
		tick = ti;

#if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
		setStatsSample(0);
#endif
	}

	void run() {
		if (client->updateNetStatus(tick)) {

			/*StringBuffer msg;
		    	msg << hex << "NETSTAT respond with 0x" << tick << "\n";
				info(msg);*/

			BasePacket* resp = new NetStatusResponseMessage(tick);
			client->sendPacket(resp);
		}
	}
};


#endif /* NETSTATUSRESPONSETASK_H_ */
