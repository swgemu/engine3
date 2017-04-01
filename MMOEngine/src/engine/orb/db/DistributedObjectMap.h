/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECTMAP_H_
#define DISTRIBUTEDOBJECTMAP_H_

#include "engine/orb/object/DistributedObject.h"

namespace engine {
  namespace ORB {

	class DistributedObjectMap {
    	HashTable<uint64, DistributedObject*> objectMap;
	
	public:
		DistributedObjectMap() {
		}

		~DistributedObjectMap() {
			auto iterator = objectMap.iterator();

			while (iterator.hasNext()) {
				auto ref = iterator.getNextValue();

				ref->release();
			}
		}
		
		bool add(sys::uint64 objectID, DistributedObject* object) {
			bool exists = objectMap.put(objectID, object) != NULL;

			if (!exists) {
				object->acquire();
			}

			return exists;
		}
	
		DistributedObject* get(sys::uint64 objectID) {
			return objectMap.get(objectID);
		}

		bool tryRemoveAndDestroy(sys::uint64 objectID) {
			DistributedObject* obj = objectMap.remove(objectID);

			if (obj) {
				bool res = obj->tryFinalRelease();

				if (!res) {
					objectMap.put(objectID, obj);

					return false;
				} else {
					return true;
				}
			}

			return false;
		}
		
		void remove(sys::uint64 objectID) {
			DistributedObject* obj = objectMap.remove(objectID);

			if (obj) {
				obj->release();
			}

			//return false;
		}

		inline int getSize() {
			return objectMap.size();
		}

		HashTable<uint64, DistributedObject* >* getMap() {
			return &objectMap;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTMAP_H_*/
