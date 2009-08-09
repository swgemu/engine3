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

	class BaseClientNetStatusCheckupEvent : public ReentrantTask {
		BaseClient* client;
		
	public:
		BaseClientNetStatusCheckupEvent(BaseClient* cl) : ReentrantTask(25000) {
			client = cl;
		}
	
		void run() {
			client->checkNetStatus();
		}

		String toString() {
			struct timespec* ts = nextExecutionTime.getTimeSpec();

			StringBuffer s;
			s << "BaseClientNetStatusCheckupEvent " << nextExecutionTime.getMiliTime() << "[" << ts->tv_sec << "/" << ts->tv_nsec << "] (ptr = " << (uint32) this << ")";
			return s.toString();
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASECLIENTNETSTATUSCHECKUPEVENT_H_*/
