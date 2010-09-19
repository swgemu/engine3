/*
 * DOBObjectManagerImplementation.h
 *
 *  Created on: 20/08/2009
 *      Author: victor
 */

#ifndef DOBOBJECTMANAGERIMPLEMENTATION_H_
#define DOBOBJECTMANAGERIMPLEMENTATION_H_

#include "engine/util/ObjectFactory.h"
#include "system/platform.h"
#include "DOBObjectManager.h"
#include "object/DistributedObjectClassHelperMap.h"

namespace engine {
	namespace ORB {

	class DistributedObjectStub;

	class DOBObjectManagerImplementation : public DOBObjectManager {
	protected:
		//DistributedObjectDirectory objectDirectory; we need a different map with object id <-> hosts

		uint64 nextObjectID;

	public:
		DOBObjectManagerImplementation() {
			nextObjectID = 0x1000000; // reserving first ids
		}

		~DOBObjectManagerImplementation() {
		}

		DistributedObjectAdapter* addObject(DistributedObjectStub* object);

		DistributedObject* loadPersistentObject(uint64 objid) {
			return NULL;
		}

		/*virtual int updatePersistentObject(DistributedObject* object) {
			return 0;
		}

		virtual DistributedObjectAdapter* addObject(DistributedObjectStub* object) {
			DistributedObjectServant* servant = object->_getImplementation();

			DistributedObjectClassHelper* helper = servant->_getClassHelper();
			DistributedObjectAdapter* adapter = helper->createAdapter(object);

			return objectDirectory.add(object->_getObjectID(), adapter);
		}*/

		DistributedObject* getObject(uint64 objectID);

		/*virtual DistributedObjectAdapter* getAdapter(uint64 objectID, bool doLock = true) {
			lock(doLock);

			DistributedObjectAdapter* adapter =  objectDirectory.getAdapter(objectID);

			unlock(doLock);

			return adapter;
		}

		virtual DistributedObjectAdapter* removeObject(uint64 objectID, bool doLock = true) {
			lock(doLock);

			DistributedObjectAdapter* object = objectDirectory.remove(objectID);

			unlock(doLock);

			return object;
		}*/

		virtual uint64 getNextFreeObjectID() {
			Locker _locker(this);

			uint64 val = ++nextObjectID;

			return val;
		}

	};

	}
}

using namespace engine::ORB;

#endif /* DOBOBJECTMANAGERIMPLEMENTATION_H_ */
