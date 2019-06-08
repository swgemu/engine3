/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * BaseClientEvent.h
 *
 *  Created on: 29/12/2009
 *      Author: victor
 */


namespace engine {
  namespace service {
    namespace proto {

	class BaseClientEvent;

    } // namespace proto
  } // namespace service
} // namespace engine

#include "engine/service/proto/BaseClient.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClientEvent : public Task {
		WeakReference<BaseClient*> client;

	public:
		BaseClientEvent(BaseClient* cl) {
			client = cl;

 #if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
			setStatsSample(0);
#endif
		}

		void run() {
			Reference<BaseClient*> strong = client.get();

			if (strong == nullptr)
				return;

			strong->run();
		}

	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#ifndef BASECLIENTEVENT_H_
#define BASECLIENTEVENT_H_


#endif /* BASECLIENTEVENT_H_ */
