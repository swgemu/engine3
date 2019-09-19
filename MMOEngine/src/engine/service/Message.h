/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

		Message(const Packet* packet, ServiceClient* clt) {
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

			stream->writeStream(getBuffer(), size);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			Packet::removeAll();

			int size;

			Integer::parseFromBinaryStream(size, stream);

			writeStream(stream, size);

			return true;
		}

		// setters and getters
		inline void setClient(ServiceClient* c) {
			client = c;
		}

		inline void setTimeStampMili(uint64 ts) {
			timestampMili = ts;
		}

		inline uint64 getTimeStampMili() const {
			return timestampMili;
		}

		inline ServiceClient* getClient() const {
			return client;
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*MESSAGE_H_*/
