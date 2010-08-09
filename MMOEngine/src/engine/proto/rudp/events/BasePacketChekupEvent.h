/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASEPACKETCHEKUPEVENT_H_
#define BASEPACKETCHEKUPEVENT_H_

namespace engine {
  namespace proto {
    namespace rudp {

	class BasePacketChekupEvent;

    } // namespace rudp
  } // namespace proto
} // namespace engine

#include "../RUDPPacket.h"
#include "../BaseClient.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class BasePacketChekupEvent : public ReentrantTask {
		RUDPProtocol* client;
		RUDPPacket* packet;

		sys::uint32 checkupTime;

	public:
		BasePacketChekupEvent(RUDPProtocol* cl, sys::uint32 time = 5000);

		void run();

		// setters and getters
		inline void update(RUDPPacket* pack) {
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

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*BASEPACKETCHEKUPEVENT_H_*/
