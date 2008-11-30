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

	class DistributedObjectClassHelperMap : public HashTable<String, DistributedObjectClassHelper*> {
		int hash(const String& key) {
	        return key.hashCode();
		}

	public:
		DistributedObjectClassHelperMap() : HashTable<String, DistributedObjectClassHelper*>(1000) {
			setNullValue(NULL);
		}

		~DistributedObjectClassHelperMap() {
			HashTableIterator<String, DistributedObjectClassHelper*> iter(this);

			while (iter.hasNext()) {
				String& helpername = iter.getNextKey();
				DistributedObjectClassHelper* helper = remove(helpername);

				helper->finalizeHelper();
			}
		}

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTCLASSHELPERMAP_H_*/
