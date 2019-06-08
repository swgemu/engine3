/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * GetNextFreeObjectID.h
 *
 *  Created on: 22/08/2009
 *      Author: victor
 */

#ifndef GETNEXTFREEOBJECTIDMESSAGE_H_
#define GETNEXTFREEOBJECTIDMESSAGE_H_

#include "system/lang.h"

#include "engine/orb/DistributedObjectBroker.h"

#include "DOBMessage.h"

namespace engine {
  namespace ORB {

	class GetNextFreeObjectIDMessage : public DOBMessage {
		bool doLock;

		uint64 objectID;

	public:
		GetNextFreeObjectIDMessage();
		GetNextFreeObjectIDMessage(Packet* message);

		void execute();

		void handleReply(Packet* response);

		uint64 getObjectID() const {
			return objectID;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;


#endif /* GETNEXTFREEOBJECTID_H_ */
