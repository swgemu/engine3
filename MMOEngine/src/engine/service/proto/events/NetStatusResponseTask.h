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
