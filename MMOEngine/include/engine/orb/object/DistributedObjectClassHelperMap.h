/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISTRIBUTEDOBJECTCLASSHELPERMAP_H_
#define DISTRIBUTEDOBJECTCLASSHELPERMAP_H_

#include "system/lang.h"

#include "DistributedObjectClassHelper.h"

namespace engine {
  namespace ORB {

	class DistributedObjectClassHelperMap : public HashTable<string, DistributedObjectClassHelper*> {
		int hash(const string& key) {
	        return sys::lang::String::hashCode(key);
		}
	
	public:
		DistributedObjectClassHelperMap() : HashTable<string, DistributedObjectClassHelper*>(1000) {
			setNullValue(NULL);
		}
		
		~DistributedObjectClassHelperMap() {
			HashTableIterator<string, DistributedObjectClassHelper*> iter(this);
			
			while (iter.hasNext()) {
				string& helpername = iter.getNextKey();
				DistributedObjectClassHelper* helper = remove(helpername);
				
				helper->finalizeHelper();
			}
		}
		
	};
	
  } // namespace ORB
} // namespace engine

using namespace engine::ORB;	

#endif /*DISTRIBUTEDOBJECTCLASSHELPERMAP_H_*/
