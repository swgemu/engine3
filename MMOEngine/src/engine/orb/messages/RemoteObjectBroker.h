/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REMOTEOBJECTBROKER_H_
#define REMOTEOBJECTBROKER_H_

#include "system/lang.h"

#include "engine/orb/ObjectBroker.h"

namespace engine {
  namespace ORB {

    class DOBServiceClient;

	class RemoteObjectBroker : public ObjectBroker {
		Reference<DOBServiceClient*> brokerClient;

	public:
		RemoteObjectBroker(const String& address, int port) {
			brokerClient = new DOBServiceClient(address, port);
			brokerClient->start();
		}

		RemoteObjectBroker(DOBServiceClient* client) {
			brokerClient = client;
		}

		~RemoteObjectBroker() {
		}

		void registerClass(const String& name, DistributedObjectClassHelper* helper) {
			assert(0);
		}

		// deployment methods
		void deploy(DistributedObjectStub* obj);
		void deploy(const String& name, DistributedObjectStub* obj);

		DistributedObjectStub* undeploy(const String& name);

		Reference<DistributedObject*> lookUp(const String& name);
		Reference<DistributedObject*> lookUp(uint64 objid);

		bool destroyObject(DistributedObjectStub* obj);

		void invokeMethod(DistributedMethod& method);
		
		DOBObjectManager* getObjectManager() {
			return NULL;
		}

		uint64 getNextFreeObjectID();

		DOBServiceClient* getBrokerClient() {
			return brokerClient;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*REMOTEOBJECTBROKER_H_*/
