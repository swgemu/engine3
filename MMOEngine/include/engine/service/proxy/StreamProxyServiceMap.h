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

	class StreamProxyServiceMap : public HashTable<string, StreamProxyService*>, 
			public HashTableIterator<string, StreamProxyService*> {
		
		int hash(const string& key) {
	        return String::hashCode(key);
		}
	
	public:
		StreamProxyServiceMap(int initsize) : HashTable<string, StreamProxyService*>((int) (initsize * 1.25f)),
				HashTableIterator<string, StreamProxyService*>() {
			setTable(this);
			setNullValue(NULL);
		}
		
		void put(const string address, int port, StreamProxyService* value) {
			string key = hashKey(address, port);

			HashTable<string, StreamProxyService*>::put(key, value);
		}

		StreamProxyService* get(const string address, int port) {
			string key = hashKey(address, port);

			return HashTable<string, StreamProxyService*>::get(key);
		}

		StreamProxyService* remove(const string address, int port) {
			string key = hashKey(address, port);

			return HashTable<string, StreamProxyService*>::remove(key);
		}
		
		/*StreamProxyService* remove(StreamProxyService* value) {
			string key = hashKey(value->getForwardingAddress(), value->getForwardingPort());

			return HashTable<string, StreamProxyService*>::remove(key);
		}*/
		
	protected:
		string hashKey(const string address, int port) {
			stringstream key;
			key << address << ":" << port;

			return key.str();
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*STREAMPROXYSERVICEMAP_H_*/
