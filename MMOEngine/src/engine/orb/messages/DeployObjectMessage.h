/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DEPLOYOBJECTMESSAGE_H_
#define DEPLOYOBJECTMESSAGE_H_

#include "system/lang.h"

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/object/DistributedObjectStub.h"

#include "engine/orb/messages/RemoteObjectBroker.h"

#include "DOBMessage.h"

namespace engine {
  namespace ORB {

	class DeployObjectMessage : public DOBMessage {
		String name;
		String className;

		bool deployed;
		uint64 objectID;

	public:	
		DeployObjectMessage(const String& name, const String& classname) : DOBMessage(DEPLOYOBJECTMESSAGE, 40) {
			insertAscii(name);
			insertAscii(classname);
		}
	
		DeployObjectMessage(Packet* message) : DOBMessage(message) {
			message->parseAscii(name);
			message->parseAscii(className);
		}

		void execute() {
			DistributedObjectBroker* broker = DistributedObjectBroker::instance();
			ObjectBroker* remoteBroker = static_cast<ObjectBroker*>(client->getRemoteObjectBroker());

			DistributedObjectStub* obj = broker->createObjectStub(className, name);
			if (obj != NULL) {
				try {
					broker->deployLocal(obj->_getName(), obj);

					obj->_setObjectBroker(remoteBroker);

					insertBoolean(true);
					insertLong(obj->_getObjectID());
				} catch (const Exception& e) {
					e.printStackTrace();

					delete obj;

					insertBoolean(false);
				}
			} else {
				delete obj;

				insertBoolean(false);
			}

			client->sendReply(this);

		}

		void handleReply(Packet* message) {
			deployed = message->parseBoolean();

			if (deployed)
				objectID = message->parseLong();
		}

		bool isDeployed() {
			return deployed;
		}

		uint64 getObjectID() {
			return objectID;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DEPLOYOBJECTMESSAGE_H_*/
