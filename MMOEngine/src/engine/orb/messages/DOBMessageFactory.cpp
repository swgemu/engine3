/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/db/DOBObjectManager.h"

#include "DeployObjectMessage.h"
#include "InvokeMethodMessage.h"
#include "LookUpObjectMessage.h"
#include "LookUpObjectByIDMessage.h"
#include "LoadPersistentObjectMessage.h"
#include "UpdatePersistentObjectMessage.h"
#include "GetNextFreeObjectIDMessage.h"
#include "UndeployObjectMessage.h"

#include "DOBMessageFactory.h"

DOBMessageFactory::DOBMessageFactory() {
}

void DOBMessageFactory::process(DOBServiceClient* client, Packet* message) {
	try {
		uint32 messageType = message->parseInt();

		System::out << "DOBMessage: " << messageType << " arrived with content: " << message->toStringData() << "\n";

		if (messageType != DOBMessage::REPLYMESSAGE) {
			DOBMessage* dobMessage = create(messageType, message);
			assert(dobMessage != NULL);

			dobMessage->setClient(client);

			dobMessage->execute();
		} else {
			DOBMessage* queuedMessage = client->getQueuedMessage(message->parseInt());
			queuedMessage->handleReply(message);

			queuedMessage->signalReply();
		}
	} catch (const StreamIndexOutOfBoundsException& e) {
		e.printStackTrace();

		System::out << e.getStream()->toStringData() << PrintStream::endl;
	} catch (const Exception& e) {
		e.printStackTrace();
	}
}

DOBMessage* DOBMessageFactory::create(uint32 messageType, Packet* message) {
	switch (messageType) {
	case DOBMessage::LOOKUPOBJECTMESSAGE:
		return new LookUpObjectMessage(message);

	case DOBMessage::LOOKUPOBJECTBYIDMESSAGE:
		return new LookUpObjectByIDMessage(message);

	case DOBMessage::DEPLOYOBJECTMESSAGE:
		return new DeployObjectMessage(message);

	case DOBMessage::UNDEPLOYOBJECTMESSAGE:
		return new UndeployObjectMessage(message);

	case DOBMessage::INVOKEMETHODMESSAGE:
		return new InvokeMethodMessage(message);

	case DOBMessage::LOADPERSISTENTOBJECTMESSAGE:
		return new LoadPersistentObjectMessage(message);
		break;

	case DOBMessage::UPDATEPERSISTENTOBJECTMESSAGE:
		return new UpdatePersistentObjectMessage(message);

	case DOBMessage::GETNEXTFREEOBJECTIDMESSAGE:
		return new GetNextFreeObjectIDMessage(message);

	default:
		return NULL;
	}
}
