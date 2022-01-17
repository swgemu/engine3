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

		uint32 checkupTime;
		uint32 minCheckupTime;
		uint32 maxCheckupTime;
		Time lastUpdateTimeStamp;

	public:
		BasePacketChekupEvent(BaseClient* cl, uint32 time = 5000, uint32 minCheckupTime = 100, uint32 maxCheckupTime = 500);

		void run();

		// setters and getters
		inline void update(BasePacket* pack) {
			packet = pack;
			lastUpdateTimeStamp.updateToCurrentTime(Time::MONOTONIC_TIME);
		}

		inline void setCheckupTime(uint32 time) {
			checkupTime = time;
		}

		void increaseCheckupTime(uint32 time) {
			if (checkupTime < maxCheckupTime)
				checkupTime += time;
		}

		void decreaseCheckupTime(uint32 time) {
			if (checkupTime > minCheckupTime)
				checkupTime -= time;
		}

		inline uint32 getCheckupTime() {
			return checkupTime;
		}

		inline int64 getElapsedTimeMs() {
			return lastUpdateTimeStamp.miliDifference(Time::MONOTONIC_TIME);
		}

	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASEPACKETCHEKUPEVENT_H_*/
