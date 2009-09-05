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
		BaseClient* client;

	public:
		BaseClientNetStatusRequestEvent(BaseClient* cl) : ReentrantTask(5000) {
			client = cl;
		}

		void run() {
			client->requestNetStatus();
		}

		String toString() {
			struct timespec* ts = nextExecutionTime.getTimeSpec();

			StringBuffer s;
			s << "BaseClientNetStatusRequestEvent " << nextExecutionTime.getMiliTime() << "[" << ts->tv_sec << "/" << ts->tv_nsec << "] (ptr = " << this << ")";
			return s.toString();
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;


#endif /* BASECLIENTNETSTATUSREQUESTEVENT_H_ */
