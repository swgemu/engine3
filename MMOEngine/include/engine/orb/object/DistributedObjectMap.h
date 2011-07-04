/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECTMAP_H_
#define DISTRIBUTEDOBJECTMAP_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class DistributedObjectAdapter;
	
	class DistributedObjectMap : public HashTable<uint64, DistributedObjectAdapter*> {
		int hash(const uint64& key) {
	        return Long::hashCode(key);
		}
	
	public:
		DistributedObjectMap() : HashTable<uint64, DistributedObjectAdapter*>(100) {
			setNullValue(NULL);
		}
		
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTMAP_H_*/
