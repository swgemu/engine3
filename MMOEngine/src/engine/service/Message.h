/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "system/lang.h"

#include "ServiceClient.h"

#include "engine/orb/object/DistributedObject.h"

namespace engine {
  namespace service {

	class Message : public Packet, public Object/*, public DistributedObject */{
	protected:
		Reference<ServiceClient*> client;
	
		uint64 timestampMili;
		
	public:
		Message() : Packet()/*, DistributedObject() */{
			client = nullptr;
			timestampMili = 0;
		}
	
		Message(int size) : Packet(size)/*, DistributedObject() */{
			client = nullptr;
			timestampMili = 0;
		}
	
		Message(Packet* packet, ServiceClient* clt) {
			packet->copy(this, 0);

			client = clt;
			timestampMili = 0;
		}

		virtual ~Message() {
		}
		
		int compareTo(Message* m) {
			uint64 t1 = timestampMili;
			uint64 t2 = m->timestampMili;
			
			if (t1 < t2)
				return 1;
			else if (t1 > t2)
				return -1;
			else
				return 0; 
		}
	
		Message* clone(int startoffs = 0) {
			Message* pack = new Message();
			copy(pack, startoffs);
			
			pack->client = client;

			return pack;
		}
	
		bool toBinaryStream(ObjectOutputStream* stream) {
			int size = Packet::size();

			Integer::toBinaryStream(size, stream);

			for (int i = 0; i < size; ++i) {
			   char* obj = &Packet::get(i);

			   TypeInfo<char>::toBinaryStream(obj, stream);
			}

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			Packet::removeAll();

			int size;

			Integer::parseFromBinaryStream(size, stream);

			for (int i = 0; i < size; ++i) {
			   char object;

			   if (TypeInfo<char>::parseFromBinaryStream(&object, stream))
				   add(object);
			}

			return true;
		}

		// setters and getters
		inline void setClient(ServiceClient* c) {
			client = c;
		}
	
		inline void setTimeStampMili(uint64 ts) {
			timestampMili = ts;
		}
	
		inline uint64 getTimeStampMili() {
			return timestampMili;
		}
	
		inline ServiceClient* getClient() {
			return client;
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*MESSAGE_H_*/
