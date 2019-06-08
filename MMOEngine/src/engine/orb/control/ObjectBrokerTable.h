/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef OBJECTBROKERTABLE_H_
#define OBJECTBROKERTABLE_H_

#include "engine/orb/ObjectBroker.h"

namespace engine {
  namespace ORB {

	class ObjectBrokerTable {
		HashSet<ObjectBroker*> objectBrokers;
	
	public:
		void add(ObjectBroker* broker) {
			objectBrokers.add(broker);
		}
	
		void remove(ObjectBroker* broker) {
			objectBrokers.remove(broker);
		}

		HashSetIterator<ObjectBroker*> iterator() {
			return objectBrokers.iterator();
		}

		int getBrokerCount() {
			return objectBrokers.size();
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*OBJECTBROKERTABLE_H_*/
