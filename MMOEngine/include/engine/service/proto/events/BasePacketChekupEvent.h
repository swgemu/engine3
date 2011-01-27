/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASEPACKETCHEKUPEVENT_H_
#define BASEPACKETCHEKUPEVENT_H_

namespace engine {
  namespace service {
    namespace proto {

	class BasePacketChekupEvent;

    } // namespace proto
  } // namespace service
} // namespace engine

#include "../BasePacket.h"
#include "../BaseClient.h"

namespace engine {
  namespace service {
    namespace proto {

	class BasePacketChekupEvent : public Task {
		BaseClient* client;
		BasePacket* packet;

		sys::uint32 checkupTime;

	public:
		BasePacketChekupEvent(BaseClient* cl, sys::uint32 time = 5000);

		void run();

		// setters and getters
		inline void update(BasePacket* pack) {
			packet = pack;
		}

		inline void setCheckupTime(sys::uint32 time) {
			checkupTime = time;
		}

		void increaseCheckupTime(sys::uint32 time) {
			if (checkupTime < 500)
				checkupTime += time;
		}

		void decreaseCheckupTime(sys::uint32 time) {
			if (checkupTime > 100)
				checkupTime -= time;
		}

		inline sys::uint32 getCheckupTime() {
			return checkupTime;
		}

	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASEPACKETCHEKUPEVENT_H_*/
