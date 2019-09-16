/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DOBMESSAGE_H_
#define DOBMESSAGE_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class DOBMessage : public Packet {
	protected:
		class DOBServiceClient* client;

		uint32 sequence;

		Condition replyCondition;

		bool async;

	public:
		enum MessageTypes { REPLYMESSAGE,
							CONTROLMESSAGE, STATEUPDATEMESSAGE,
							LOOKUPOBJECTMESSAGE, LOOKUPOBJECTBYIDMESSAGE,
							DEPLOYOBJECTMESSAGE, UNDEPLOYOBJECTMESSAGE,
							INVOKEMETHODMESSAGE,
							LOADPERSISTENTOBJECTMESSAGE, UPDATEPERSISTENTOBJECTMESSAGE,
							GETNEXTFREEOBJECTIDMESSAGE, REQUESTSERVANT, SENDOBJECTDATA };

	public:
		DOBMessage(uint32 messageType);
		DOBMessage(uint32 messageType, uint32 initialBufferSize);
		DOBMessage(Packet* packet);

		~DOBMessage();

		virtual void execute() = 0;

		virtual void handleReply(Packet* response) {
		}

		void signalReply();

		bool waitForReply(uint32 timeout = 10000);

		DOBServiceClient* getClient() {
			return client;
		}

		const DOBServiceClient* getClient() const {
			return client;
		}

		uint32 getSequence() const {
			return sequence;
		}

		void setClient(DOBServiceClient* cli) {
			client = cli;
		}

		void setSequence(uint32 seq) {
			sequence = seq;
			insertInt(8, seq); // sequence
		}

		void setSize() {
			insertInt(0, size());
		}

		bool isAsync() const {
			return async;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DOBMESSAGE_H_*/
