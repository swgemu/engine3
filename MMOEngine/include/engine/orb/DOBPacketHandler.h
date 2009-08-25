/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DOBPACKETHANDLER_H_
#define DOBPACKETHANDLER_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

namespace engine {
  namespace ORB {

	class DistributedObjectBroker;
	class DistributedObjectBrokerClient;
	
	class DOBPacketHandler : public Logger {
		DistributedObjectBroker* orb;
	
	public:
		DOBPacketHandler();
		DOBPacketHandler(const String& s, DistributedObjectBroker* broker);
	
		~DOBPacketHandler();
	
		void handlePacket(DistributedObjectBrokerClient* client, Packet* pack);
		
		void handleObjectLookUpMessage(DistributedObjectBrokerClient* client, Packet* pack);
		void handleObjectLookUpMessageByID(DistributedObjectBrokerClient* client, Packet* pack);

		void handleObjectDeployMessage(DistributedObjectBrokerClient* client, Packet* pack);
		void handleMethodInvocationMessage(DistributedObjectBrokerClient* client, Packet* pack);
		void handleLoadPersistentObjectMessage(DistributedObjectBrokerClient* client, Packet* pack);

		void handleUpdatePersistentObjectMessage(DistributedObjectBrokerClient* client, Packet* pack);
		void handleGetNextFreeObjectIDMessage(DistributedObjectBrokerClient* client, Packet* pack);
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DOBPACKETHANDLER_H_*/
