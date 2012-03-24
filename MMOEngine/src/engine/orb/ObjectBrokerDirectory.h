/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECTBROKER_H_
#define OBJECTBROKER_H_

#include "system/lang.h"

#include "ObjectBroker.h"

namespace engine {
  namespace ORB {

	class ObjectBrokerDirectory {
    	HashTable<uint64, ObjectBroker*> objectMap;

	public:
    	ObjectBrokerDirectory();

		virtual ~ObjectBroker() {
		}

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*OBJECTBROKER_H_*/
