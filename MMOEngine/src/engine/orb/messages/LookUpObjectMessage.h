/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef LOOKUPOBJECTMESSAGE_H_
#define LOOKUPOBJECTMESSAGE_H_

#include "system/lang.h"

#include "engine/orb/DistributedObjectBroker.h"

#include "DOBMessage.h"

namespace engine {
  namespace ORB {

	class LookUpObjectMessage : public DOBMessage {
		String name;

		bool found;
		String className;
		uint64 objectID;

	public:	
		LookUpObjectMessage(const String& name) : DOBMessage(LOOKUPOBJECTMESSAGE, 40) {
			insertAscii(name);

			found = false;
			objectID = 0;
		}

		LookUpObjectMessage(Packet* message) : DOBMessage(message) {
			message->parseAscii(name);

			found = false;
			objectID = 0;
		}

		void execute() {
			DistributedObject* obj = DistributedObjectBroker::instance()->lookUp(name);

			if (obj != nullptr) {
				insertBoolean(true);
				insertAscii(obj->_getClassName());
				insertLong(obj->_getObjectID());
			} else {
				insertBoolean(false);
			}

			client->sendReply(this);
		}

		void handleReply(Packet* message) {
			found = message->parseBoolean();

			if (found) {
				message->parseAscii(className);

				objectID = message->parseLong();
			}
		}

		bool isFound() {
			return found;
		}

		const String& getClassName() {
			return className;
		}

		uint64 getObjectID() {
			return objectID;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*LOOKUPOBJECTMESSAGE_H_*/
