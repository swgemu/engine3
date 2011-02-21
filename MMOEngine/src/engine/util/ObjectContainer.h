/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECTCONTAINER_H_
#define OBJECTCONTAINER_H_

#include "system/lang.h"

namespace engine {
	namespace util {

	template<class K> class KeyHandler : public Object {
	public:
		virtual K generateKey() = 0;
	};

	template<class K, class O> class ObjectContainer {
		HashTable<K, Reference<O> > objects;

		Reference<KeyHandler<K>*> keyHandler;

	public:
		ObjectContainer(KeyHandler<K>* handler) : objects(100000) {
			keyHandler = handler;

			objects.setNullValue(NULL);
		}

		bool add(const O& object) {
			K key = keyHandler->generateKey();

			return add(key, object);
		}

		bool add(const K& key, const O& object) {
			return objects.put(key, object) == NULL;
		}

		O& get(const K& key) const {
			return objects.get(key);
		}

		O remove(const K& key) {
			return objects.remove(key);
		}

		KeyHandler<K>* getKeyHandler() {
			return keyHandler;
		}
};


	} // namespace util
} // namespace engine

using namespace engine::util;

#endif /*OBJECTCONTAINER_H_*/
