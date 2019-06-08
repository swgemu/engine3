/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISTRIBUTEDOBJECTCLASSHELPERMAP_H_
#define DISTRIBUTEDOBJECTCLASSHELPERMAP_H_

#include "system/lang.h"

#include "DistributedObjectClassHelper.h"

namespace engine {
  namespace ORB {

	class DistributedObjectClassHelperMap : public HashTable<String, DistributedObjectClassHelper*> {
	public:
		DistributedObjectClassHelperMap() : HashTable<String, DistributedObjectClassHelper*>(1000) {
		}

		~DistributedObjectClassHelperMap() {
			HashTableIterator<String, DistributedObjectClassHelper*> iter(this);

			while (iter.hasNext()) {
				String& helpername = iter.getNextKey();
				DistributedObjectClassHelper* helper = remove(helpername);

				//TODO: fix crash
				//helper->finalizeHelper();
			}
		}

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DISTRIBUTEDOBJECTCLASSHELPERMAP_H_*/
