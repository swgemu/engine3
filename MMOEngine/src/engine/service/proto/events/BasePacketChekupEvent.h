/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BASEPACKETCHEKUPEVENT_H_
#define BASEPACKETCHEKUPEVENT_H_

#include "engine/core/Task.h"

namespace engine {
  namespace service {
    namespace proto {

	class BasePacketChekupEvent;

    } // namespace proto
  } // namespace service
} // namespace engine


namespace engine {
  namespace service {
    namespace proto {
	class BaseClient;
	class BasePacket;

	class BasePacketChekupEvent : public Task {
		WeakReference<BaseClient*> client;
		Reference<BasePacket*> packet;

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
