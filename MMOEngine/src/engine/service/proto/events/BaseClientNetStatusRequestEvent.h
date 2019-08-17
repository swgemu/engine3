/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * BaseClientNetStatusRequestEvent.h
 *
 *  Created on: Sep 5, 2009
 *      Author: theanswer
 */

#ifndef BASECLIENTNETSTATUSREQUESTEVENT_H_
#define BASECLIENTNETSTATUSREQUESTEVENT_H_

#include "engine/service/proto/BasePacket.h"
#include "engine/service/proto/BaseClient.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClientNetStatusRequestEvent : public Task {
		Reference<BaseClient*> client;
		Mutex lock;

	public:
		BaseClientNetStatusRequestEvent(BaseClient* cl) : Task(5000) {
			client = cl;

#if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
			setStatsSample(0);
#endif
		}

		void run() {
			Locker locker(&lock);

			auto strongRef = client;

			if (strongRef != nullptr)
				strongRef->requestNetStatus();
		}

		void clearClient() {
			Locker locker(&lock);

			client = nullptr;
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;


#endif /* BASECLIENTNETSTATUSREQUESTEVENT_H_ */
