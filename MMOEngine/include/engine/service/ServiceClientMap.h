/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERVICECLIENTMAP_H_
#define SERVICECLIENTMAP_H_

#include "../../system/lang.h"

#include "ServiceClient.h"

namespace engine {
  namespace service {

	class ServiceClientMap : public HashTable<uint64, ServiceClient*>, 
			public HashTableIterator<uint64, ServiceClient*> {
				
		int maxConnections;
	
		int hash(const uint64& key) {
	        return Long::hashCode(key);
		}
	
	public:
		ServiceClientMap(int maxconn) : HashTable<uint64, ServiceClient*>((int) (maxconn * 1.25f)),
				HashTableIterator<uint64, ServiceClient*>(this) {
			maxConnections = maxconn;
	
			setNullValue(NULL);
		}

		bool add(ServiceClient* client) {
			return HashTable<uint64, ServiceClient*>::put(client->getNetworkID(), client) == NULL;
		}

		bool remove(ServiceClient* client) {
			return HashTable<uint64, ServiceClient*>::remove(client->getNetworkID()) != NULL;
		}
		
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICECLIENTMAP_H_*/
