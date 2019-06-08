/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * UpdatePersistentObjectMessage.h
 *
 *  Created on: 21/08/2009
 *      Author: victor
 */

#ifndef UPDATEPERSISTENTOBJECTMESSAGE_H_
#define UPDATEPERSISTENTOBJECTMESSAGE_H_

#include "engine/orb/DistributedObjectBroker.h"

#include "DOBMessage.h"

namespace engine {
  namespace ORB {

	  class UpdatePersistentObjectMessage : public DOBMessage {
		  uint64 objectID;

		  uint32 success;

	  public:
		  UpdatePersistentObjectMessage(DistributedObject* object) : DOBMessage(UPDATEPERSISTENTOBJECTMESSAGE, 40) {
			  insertLong(object->_getObjectID());

			  success = 0;
		  }

		  UpdatePersistentObjectMessage(Packet* message) : DOBMessage(message) {
			  objectID = message->parseLong();

			  success = 0;
		  }

		  void execute() {
			  DistributedObjectBroker* orb = DistributedObjectBroker::instance();
			  DOBObjectManager* objectManager = orb->getObjectManager();

			  DistributedObject* obj = objectManager->getObject(objectID);

			  uint32 success;

			  if (obj != nullptr) {
				  orb->error("could not locate object in  DOBMessageFactory::handleUpdatePersistentObjectMessage");

				  success = 0;
			  } else {
				  success = objectManager->updatePersistentObject(obj);
			  }

			  insertInt(success);
			  client->sendReply(this);
		  }

		  void handleReply(Packet* response) {
			  success = response->parseInt();
		  }
	  };

  }
}

using namespace engine::ORB;


#endif /* UPDATEPERSISTENTOBJECTMESSAGE_H_ */
