/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

		String name;
		String className;
		bool found;

	public:
		LookUpObjectByIDMessage(uint64 objectid) : DOBMessage(LOOKUPOBJECTBYIDMESSAGE, 20), objectid(objectid) {
			insertLong(objectid);

			found = false;
		}

		LookUpObjectByIDMessage(Packet* message) : DOBMessage(message) {
			objectid = message->parseLong();

			found = false;
		}

		void execute() {
			DistributedObjectBroker* broker = DistributedObjectBroker::instance();
			DOBObjectManager* objectManager = broker->getObjectManager();

			DistributedObject* obj = objectManager->getObject(objectid);

			if (obj != nullptr) {
				insertBoolean(true);
				insertAscii(obj->_getClassName());
				insertAscii(obj->_getName());

				broker->debug() << "looked up 0x" << objectid << " with name \'"
						<< obj->_getName() << "\' (" << obj->_getClassName() << ")";
			} else
				insertBoolean(false);

			client->sendReply(this);
		}

		void handleReply(Packet* message) {
			found = message->parseBoolean();

			if (found) {
				message->parseAscii(className);
				message->parseAscii(name);
			}
		}

		const String& getClassName() const {
			return className;
		}

		const String& getName() const {
			return name;
		}

		bool isFound() const {
			return found;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*LOOKUPOBJECTBYIDMESSAGE_H_*/
