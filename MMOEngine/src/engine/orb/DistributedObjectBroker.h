/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISTRIBUTEDOBJECTBROKER_H_
#define DISTRIBUTEDOBJECTBROKER_H_

#include "system/lang.h"

#include "engine/util/Singleton.h"

#include "engine/service/StreamServiceThread.h"

#include "ObjectBroker.h"

#include "NamingDirectoryService.h"

#include "messages/DOBServiceClient.h"

#include "object/DistributedObject.h"
#include "object/DistributedObjectStub.h"
#include "object/DistributedObjectServant.h"
#include "object/DistributedObjectAdapter.h"

#include "object/DistributedObjectClassHelper.h"
#include "object/DistributedObjectClassHelperMap.h"

#include "object/DistributedMethod.h"

#include "db/DistributedObjectMap.h"

namespace engine {
  namespace core {
  	class Core;
  }

  namespace ORB {

	class DOBObjectManager;

	class DistributedObjectBroker : public StreamServiceThread, public ObjectBroker, public Singleton<DistributedObjectBroker> {
		ObjectBroker* rootObjectBroker;

		NamingDirectoryService* namingDirectoryService;

		DistributedObjectClassHelperMap classMap;

		DOBObjectManager* objectManager;

		DistributedObjectMap<HashTable<uint64, DistributedObject*>> remoteObjectCache;

		String address;
		int port;

	public:
		static const int NAMING_DIRECTORY_PORT = 44459;

	protected:
		DistributedObjectBroker();

		virtual ~DistributedObjectBroker();

	public:
		static DistributedObjectBroker* initialize(const String& addr, int port = 44433);

		void initialize();

		void run();

		void shutdown();

		void registerClass(const String& name, DistributedObjectClassHelper* helper);

		// deployment methods
		void deploy(DistributedObjectStub* obj);
		void deploy(const String& name, DistributedObjectStub* obj);

		void requestServant(DistributedObjectStub* obj);

		bool destroyObject(DistributedObjectStub* obj);

		Reference<DistributedObject*> lookUp(const String& name);
		Reference<DistributedObject*> lookUp(uint64 objid);

		DistributedObjectStub* undeploy(const String& name);

		DistributedObjectStub* createObjectStub(const String& className, const String& name);
		DistributedObjectServant* createObjectServant(const String& className, DistributedObjectStub* stub, bool addToObjectManager = false);

		DistributedObjectPOD* createObjectPOD(const String& className);

		DistributedObjectAdapter* getObjectAdapter(const String& name);
		DistributedObjectAdapter* getObjectAdapter(uint64 oid);

		uint64 getNextFreeObjectID();

		// getters
		ObjectBroker* getRootBroker() {
			return rootObjectBroker;
		}

		bool isRootBroker() {
			return rootObjectBroker == nullptr;
		}

		/*inline DistributedObjectClassHelperMap* getClassMap() {
			return &classMap;
		}*/

		DOBObjectManager* getObjectManager() {
			return objectManager;
		}

		// setters
		void setCustomObjectManager(DOBObjectManager* manager);

	private:
		void deployLocal(const String& name, DistributedObjectStub* obj);
		void deployRemote(const String& name, DistributedObjectStub* obj);

		DistributedObjectStub* undeployLocal(const String& name);
		DistributedObjectStub* undeployRemote(const String& name);

		friend class engine::core::Core;
		friend class NamingDirectoryService;
		friend class RemoteObjectBroker;

		friend class DeployObjectMessage;
		friend class UndeployObjectMessage;

		friend class SingletonWrapper<DistributedObjectBroker>;
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTBROKER_H_*/
