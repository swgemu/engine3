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

	class DatagramProxyServiceMap : public HashTable<String, DatagramProxyService*>,
			public HashTableIterator<String, DatagramProxyService*> {

		int hash(const String& key) {
	        return key.hashCode();
		}

	public:
		DatagramProxyServiceMap(int initsize) : HashTable<String, DatagramProxyService*>((int) (initsize * 1.25f)),
				HashTableIterator<String, DatagramProxyService*>() {
			setTable(this);
			setNullValue(NULL);
		}

		void put(const String address, int port, DatagramProxyService* value) {
			String key = hashKey(address, port);

			HashTable<String, DatagramProxyService*>::put(key, value);
		}

		DatagramProxyService* get(const String address, int port) {
			String key = hashKey(address, port);

			return HashTable<String, DatagramProxyService*>::get(key);
		}

		DatagramProxyService* remove(const String address, int port) {
			String key = hashKey(address, port);

			return HashTable<String, DatagramProxyService*>::remove(key);
		}

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

#endif /*DATAGRAMPROXYSERVICEMAP_H_*/
