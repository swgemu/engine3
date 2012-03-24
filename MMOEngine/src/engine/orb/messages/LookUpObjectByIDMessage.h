/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOOKUPOBJECTBYIDMESSAGE_H_
#define LOOKUPOBJECTBYIDMESSAGE_H_

#include "system/lang.h"

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/db/DOBObjectManager.h"

#include "DOBMessage.h"

namespace engine {
  namespace ORB {

	class LookUpObjectByIDMessage : public DOBMessage {
		uint64 objectid;

		bool found;

	public:	
		LookUpObjectByIDMessage(uint64 objectid) : DOBMessage(LOOKUPOBJECTBYIDMESSAGE, 20) {
			insertLong(objectid);
		}
	
		LookUpObjectByIDMessage(Packet* message) : DOBMessage(message) {
			objectid = message->parseLong();
		}

		void execute() {
			DOBObjectManager* objectManager = DistributedObjectBroker::instance()->getObjectManager();

			DistributedObject* obj = objectManager->getObject(objectid);

			if (obj != NULL) {
				insertBoolean(true);
				insertAscii(obj->_getClassName());
				insertLong(obj->_getObjectID());
			} else
				insertByte(0);

			client->sendReply(this);
		}

		void handleReply(Packet* message) {
			found = message->parseBoolean();
		}

		bool isFound() {
			return found;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*LOOKUPOBJECTBYIDMESSAGE_H_*/
