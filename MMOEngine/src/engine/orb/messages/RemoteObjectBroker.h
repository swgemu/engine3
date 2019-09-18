/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef REMOTEOBJECTBROKER_H_
#define REMOTEOBJECTBROKER_H_

#include "system/lang.h"

#include "engine/orb/ObjectBroker.h"

#include "system/util/SynchronizedSortedVector.h"

namespace engine {
  namespace ORB {

	class DOBServiceClient;
	class DistributedMethod;

	class RemoteObjectBroker : public ObjectBroker, public Mutex, public Logger {
		Reference<DOBServiceClient*> brokerClient;
		SynchronizedSortedVector<DistributedObject*> deployedObjects;

	public:
		RemoteObjectBroker(const String& address, int port);
		RemoteObjectBroker(DOBServiceClient* client);

		~RemoteObjectBroker();

		void registerClass(const String& name, DistributedObjectClassHelper* helper);

		// deployment methods
		void deploy(DistributedObjectStub* obj);
		void deploy(const String& name, DistributedObjectStub* obj);

		void requestServant(DistributedObjectStub* obj);

		DistributedObjectStub* undeploy(const String& name);

		Reference<DistributedObject*> lookUp(const String& name);
		Reference<DistributedObject*> lookUp(uint64 objid);

		bool destroyObject(DistributedObjectStub* obj);

		void invokeMethod(DistributedMethod& method, bool async);

		void addDeployedObject(DistributedObject* obj);
		void removeDeployedObject(DistributedObject* obj);

		SynchronizedSortedVector<DistributedObject*>& getDeployedObjects() {
			return deployedObjects;
		}

		DOBObjectManager* getObjectManager() {
			return nullptr;
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
