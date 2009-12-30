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

#include "../BaseClient.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClientEvent : public ReentrantTask {
		BaseClient* client;

	public:
		BaseClientEvent(BaseClient* cl) {
			client = cl;
		}

		void run() {
			client->run();
		}

	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#ifndef BASECLIENTEVENT_H_
#define BASECLIENTEVENT_H_


#endif /* BASECLIENTEVENT_H_ */
