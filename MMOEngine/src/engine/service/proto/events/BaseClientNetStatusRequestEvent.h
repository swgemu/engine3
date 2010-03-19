/*
 * BaseClientNetStatusRequestEvent.h
 *
 *  Created on: Sep 5, 2009
 *      Author: theanswer
 */

#ifndef BASECLIENTNETSTATUSREQUESTEVENT_H_
#define BASECLIENTNETSTATUSREQUESTEVENT_H_

#include "../BasePacket.h"
#include "../BaseClient.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClientNetStatusRequestEvent : public ReentrantTask {
		Reference<BaseClient> client;

	public:
		BaseClientNetStatusRequestEvent(BaseClient* cl) : ReentrantTask(5000) {
			client = cl;
		}

		void run() {
			client->requestNetStatus();
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;


#endif /* BASECLIENTNETSTATUSREQUESTEVENT_H_ */
