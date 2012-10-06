/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASECLIENTNETSTATUSCHECKUPEVENT_H_
#define BASECLIENTNETSTATUSCHECKUPEVENT_H_

#include "../BasePacket.h"
#include "../BaseClient.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClientNetStatusCheckupEvent : public Task {
		Reference<BaseClient*> client;
		Mutex lock;

	public:
		BaseClientNetStatusCheckupEvent(BaseClient* cl) : Task(25000) {
			client = cl;
		}

		void run() {
			Locker locker(&lock);

			if (client != NULL)
				client->checkNetStatus();
		}

		void clearClient() {
			Locker locker(&lock);

			client = NULL;
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASECLIENTNETSTATUSCHECKUPEVENT_H_*/
