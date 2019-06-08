/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * LoadPersistentObjectMessage.h
 *
 *  Created on: 20/08/2009
 *      Author: victor
 */

#ifndef LOADPERSISTENTOBJECTMESSAGE_H_
#define LOADPERSISTENTOBJECTMESSAGE_H_

#include "engine/orb/DistributedObjectBroker.h"

#include "DOBMessage.h"

namespace engine {
  namespace ORB {

	class LoadPersistentObjectMessage : public DOBMessage {
		uint64 objectID;

		bool found;

	public:
		LoadPersistentObjectMessage(uint64 objectid) : DOBMessage(LOADPERSISTENTOBJECTMESSAGE, 40), objectID(objectid) {
			insertLong(objectid);

			found = false;
		}

		LoadPersistentObjectMessage(Packet* message) : DOBMessage(message) {
			objectID = message->parseLong();

			found = false;
		}

		void execute() {
			DistributedObjectBroker* orb = DistributedObjectBroker::instance();
			DOBObjectManager* objectManager = orb->getObjectManager();

			DistributedObject* obj = objectManager->loadPersistentObject(objectID);

			if (obj != nullptr) {
				insertBoolean(true);
				insertAscii(obj->_getClassName());
				insertLong(obj->_getObjectID());
			} else
				insertByte(0);

			client->sendReply(this);
		}

		void handleReply(Packet* response) {
			found = response->parseBoolean();
		}

		bool isFound() {
			return found;
		}
	};

  }
}

using namespace engine::ORB;


#endif /* LOADPERSISTENTOBJECTMESSAGE_H_ */
