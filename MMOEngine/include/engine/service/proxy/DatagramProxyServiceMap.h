/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMPROXYSERVICEMAP_H_
#define DATAGRAMPROXYSERVICEMAP_H_

#include "system/lang.h"

#include "DatagramProxyService.h"

namespace engine {
  namespace service {

	class DatagramProxyServiceMap : public HashTable<string, DatagramProxyService*>,
			public HashTableIterator<string, DatagramProxyService*> {
				
		int hash(const string& key) {
	        return String::hashCode(key);
		}
	
	public:
		DatagramProxyServiceMap(int initsize) : HashTable<string, DatagramProxyService*>((int) (initsize * 1.25f)), 
				HashTableIterator<string, DatagramProxyService*>() {
			setTable(this);
			setNullValue(NULL);
		}

		void put(const string address, int port, DatagramProxyService* value) {
			string key = hashKey(address, port);

			HashTable<string, DatagramProxyService*>::put(key, value);
		}

		DatagramProxyService* get(const string address, int port) {
			string key = hashKey(address, port);

			return HashTable<string, DatagramProxyService*>::get(key);
		}

		DatagramProxyService* remove(const string address, int port) {
			string key = hashKey(address, port);

			return HashTable<string, DatagramProxyService*>::remove(key);
		}
		
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

#endif /*DATAGRAMPROXYSERVICEMAP_H_*/
