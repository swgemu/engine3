/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERVICECLIENTMAP_H_
#define SERVICECLIENTMAP_H_

#include "system/lang.h"

#include "ServiceSession.h"

namespace engine {
  namespace service {

	class ServiceSessionMap : public HashTable<uint64, ServiceSession*>, 
			public HashTableIterator<uint64, ServiceSession*> {
				
		int maxConnections;
	
		int hash(const uint64& key) {
	        return Long::hashCode(key);
		}
	
	public:
		ServiceSessionMap(int maxconn) : HashTable<uint64, ServiceSession*>((int) (maxconn * 1.25f)),
				HashTableIterator<uint64, ServiceSession*>(this) {
			maxConnections = maxconn;
	
			setNullValue(NULL);
		}

		bool add(ServiceSession* client) {
			if (HashTable<uint64, ServiceSession*>::put(client->getNetworkID(), client) == NULL) {
				client->acquire();

				return true;
			} else
				return false;
		}

		bool remove(ServiceSession* client) {
			if (HashTable<uint64, ServiceSession*>::remove(client->getNetworkID()) != NULL) {
				client->release();

				return true;
			} else
				return false;
		}
		
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICECLIENTMAP_H_*/
