/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECTCONTAINER_H_
#define OBJECTCONTAINER_H_

#include "system/lang.h"

namespace engine {
	namespace util {

	template<class K> class ObjectContainer : public HashTable<K, V> {
		int hash(const String& key) {
	        return key.hashCode();
		}

	public:
		ObjectContainer() : HashTable<String, DistributedObjectStub*>(100) {
			setNullValue(NULL);
		}

	};


	} // namespace util
} // namespace engine

using namespace engine::util;

#endif /*OBJECTCONTAINER_H_*/
