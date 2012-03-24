/*
 * DOBObjectManager.h
 *
 *  Created on: 18/08/2009
 *      Author: victor
 */

#ifndef DOBOBJECTMANAGER_H_
#define DOBOBJECTMANAGER_H_

#include "system/platform.h"

#include "engine/util/ObjectFactory.h"

#include "DistributedObjectDirectory.h"

namespace engine {
	namespace ORB {

	class DistributedObject;
	class DistributedObjectAdapter;
	class DistributedObjectStub;
	class DOBServiceClient;

	class DOBObjectManager : public Mutex {
	protected:
		DistributedObjectDirectory localObjectDirectory;

		uint64 nextObjectID;

	public:
		DOBObjectManager();

		virtual ~DOBObjectManager() {
			//localObjectDirectory.destroyContainingObjects();
		}

		DistributedObject* loadPersistentObject(uint64 objid) {
			return NULL;
		}

		void updateModifiedObjectsToDatabase();

		int updatePersistentObject(DistributedObject* object);

		DistributedObjectAdapter* addObject(DistributedObjectStub* object);

		DistributedObject* getObject(uint64 objectID);

		DistributedObjectAdapter* getAdapter(uint64 objectID);

		DistributedObjectAdapter* removeObject(uint64 objectID);

		virtual void createObjectID(const String& name, DistributedObjectStub* object);

		uint64 getNextFreeObjectID();

		//virtual void savePersistentObjects();
	};

	}
}

using namespace engine::ORB;

#endif /* DOBOBJECTMANAGER_H_ */
