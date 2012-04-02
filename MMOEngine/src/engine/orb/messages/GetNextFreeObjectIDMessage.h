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
		GetNextFreeObjectIDMessage() : DOBMessage(GETNEXTFREEOBJECTIDMESSAGE, 20) {
		}

		GetNextFreeObjectIDMessage(Packet* message) : DOBMessage(message) {
		}

		void execute() {
			DistributedObjectBroker* broker = DistributedObjectBroker::instance();

			uint64 objectID = broker->getNextFreeObjectID();

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
