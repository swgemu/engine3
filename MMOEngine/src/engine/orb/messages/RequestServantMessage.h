/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 15/10/16.
//

#ifndef ENGINE3_REQUESTSERVANTMESSAGE_H
#define ENGINE3_REQUESTSERVANTMESSAGE_H

#include "system/lang.h"

#include "engine/orb/DistributedObjectBroker.h"

#include "DOBMessage.h"

namespace engine {
	namespace ORB {

		class RequestServantMessage : public DOBMessage {
			uint64 objectID;

		public:
			RequestServantMessage(uint64 oid);
			RequestServantMessage(Packet* message);

			void execute();

			void handleReply(Packet* message);

			uint64 getObjectID() const {
				return objectID;
			}
		};

	} // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif //ENGINE3_REQUESTSERVANTMESSAGE_H
