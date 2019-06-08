/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 22/10/16.
//

#ifndef ENGINE3_SENDOBJECTDATAMESSAGE_H
#define ENGINE3_SENDOBJECTDATAMESSAGE_H


#include "system/lang.h"

#include "engine/orb/DistributedObjectBroker.h"

#include "DOBMessage.h"

namespace engine {
	namespace ORB {

		class SendObjectDataMessage : public DOBMessage {
			uint64 objectID;
			Stream* objectData;

		public:
			SendObjectDataMessage(uint64 oid, Stream* data);
			SendObjectDataMessage(Packet* message);

			void execute();

			void handleReply(Packet* message);

			uint64 getObjectID() const {
				return objectID;
			}
		};

	} // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif //ENGINE3_SENDOBJECTDATAMESSAGE_H
