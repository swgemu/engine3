/*
 * BaseClientNetStatusRequestEvent.h
 *
 *  Created on: Sep 5, 2009
 *      Author: theanswer
 */

#ifndef BASECLIENTNETSTATUSREQUESTEVENT_H_
#define BASECLIENTNETSTATUSREQUESTEVENT_H_

#include "../RUDPPacket.h"
#include "../BaseClient.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class BaseClientNetStatusRequestEvent : public ReentrantTask {
		Reference<RUDPProtocol*> client;

	public:
		BaseClientNetStatusRequestEvent(RUDPProtocol* cl) : ReentrantTask(5000) {
			client = cl;
		}

		void run() {
			client->requestNetStatus();
		}
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /* BASECLIENTNETSTATUSREQUESTEVENT_H_ */
