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

		DistributedObject* getObject(uint64 objectID);

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
