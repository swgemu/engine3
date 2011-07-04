/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STREAMPROXYSERVICEMAP_H_
#define STREAMPROXYSERVICEMAP_H_

#include "system/lang.h"

#include "StreamProxyService.h"

namespace engine {
  namespace service {

	class StreamProxyServiceMap : public HashTable<String, StreamProxyService*>,
			public HashTableIterator<String, StreamProxyService*> {

		int hash(const String& key) {
	        return key.hashCode();
		}

	public:
		StreamProxyServiceMap(int initsize) : HashTable<String, StreamProxyService*>((int) (initsize * 1.25f)),
				HashTableIterator<String, StreamProxyService*>() {
			setTable(this);
			setNullValue(NULL);
		}

		void put(const String address, int port, StreamProxyService* value) {
			String key = hashKey(address, port);

			HashTable<String, StreamProxyService*>::put(key, value);
		}

		StreamProxyService* get(const String address, int port) {
			String key = hashKey(address, port);

			return HashTable<String, StreamProxyService*>::get(key);
		}

		StreamProxyService* remove(const String address, int port) {
			String key = hashKey(address, port);

			return HashTable<String, StreamProxyService*>::remove(key);
		}

		/*StreamProxyService* remove(StreamProxyService* value) {
			String key = hashKey(value->getForwardingAddress(), value->getForwardingPort());

			return HashTable<String, StreamProxyService*>::remove(key);
		}*/

	protected:
		String hashKey(const String address, int port) {
			StringBuffer key;
			key << address << ":" << port;

			return key.toString();
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*STREAMPROXYSERVICEMAP_H_*/
