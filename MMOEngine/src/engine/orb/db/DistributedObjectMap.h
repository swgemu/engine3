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
    	HashTable<uint64, Reference<DistributedObject*> > objectMap;
	
	public:
		DistributedObjectMap() {
		}

		~DistributedObjectMap() {
		}
		
		DistributedObject* add(sys::uint64 objectID, DistributedObject* object) {
			return objectMap.put(objectID, object);
		}
	
		DistributedObject* get(sys::uint64 objectID) {
			return objectMap.get(objectID);
		}
		
		DistributedObject* remove(sys::uint64 objectID) {
			return objectMap.remove(objectID);
		}

		inline int getSize() {
			return objectMap.size();
		}

		HashTable<uint64, Reference<DistributedObject*> >* getMap() {
			return &objectMap;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTMAP_H_*/
