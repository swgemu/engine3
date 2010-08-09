/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASECLIENTEVENT_H_
#define BASECLIENTEVENT_H_

namespace engine {
  namespace proto {
    namespace rudp {

	class BaseClientEvent;

    } // namespace rudp
  } // namespace proto
} // namespace engine

#include "../BaseClient.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class BaseClientEvent : public ReentrantTask {
		RUDPProtocol* client;

	public:
		BaseClientEvent(RUDPProtocol* cl) {
			client = cl;
		}

		void run() {
			client->run();
		}

	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /* BASECLIENTEVENT_H_ */
