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
		GetNextFreeObjectIDMessage(bool doLock) : DOBMessage(GETNEXTFREEOBJECTIDMESSAGE, 10) {
			insertBoolean(doLock);
		}

		GetNextFreeObjectIDMessage(Packet* message) : DOBMessage(message) {
			doLock = message->parseBoolean();
		}

		void execute() {
			DistributedObjectBroker* orb = DistributedObjectBroker::instance();
			DOBObjectManager* objectManager = orb->getObjectManager();

			uint64 objectID = objectManager->getNextFreeObjectID();

			insertLong(objectID);
			client->sendReply(this);
		}

		void handleReply(Packet* response) {
			objectID = response->parseLong();
		}

		uint64 getObjectID() {
			return objectID;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;


#endif /* GETNEXTFREEOBJECTID_H_ */
