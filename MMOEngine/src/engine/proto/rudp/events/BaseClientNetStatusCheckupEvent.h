/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASECLIENTNETSTATUSCHECKUPEVENT_H_
#define BASECLIENTNETSTATUSCHECKUPEVENT_H_

#include "../RUDPPacket.h"
#include "../BaseClient.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class BaseClientNetStatusCheckupEvent : public ReentrantTask {
		Reference<RUDPProtocol*> client;

	public:
		BaseClientNetStatusCheckupEvent(RUDPProtocol* cl) : ReentrantTask(25000) {
			client = cl;
		}

		void run() {
			client->checkNetStatus();
		}
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*BASECLIENTNETSTATUSCHECKUPEVENT_H_*/
