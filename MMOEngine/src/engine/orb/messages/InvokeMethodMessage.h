/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef INVOKEMETHODMESSAGE_H_
#define INVOKEMETHODMESSAGE_H_

#include "system/lang.h"

#include "engine/orb/DistributedObjectBroker.h"

#include "DOBMessage.h"

namespace engine {
  namespace ORB {

	class InvokeMethodMessage : public DOBMessage {
		uint64 objectID;
		uint32 methodID;
		uint32 invocationID;

		Packet* packet;

	public:	
		InvokeMethodMessage(uint64 objectid, sys::uint32 methid, sys::uint32 invid) : DOBMessage(INVOKEMETHODMESSAGE, 40) {
			insertLong(objectid);

			insertInt(methid);
			insertInt(invid);

			packet = NULL;
			objectID = 0;
			methodID = 0;
			invocationID = 0;
		}
		
		InvokeMethodMessage(Packet* message) : DOBMessage(message) {
			objectID = message->parseLong();

			methodID = message->parseInt();
			invocationID = message->parseInt();

			packet = message->clone();
		}

		~InvokeMethodMessage() {
			if (packet != NULL)
				delete packet;
		}

		void execute() {
			DistributedObjectBroker* orb = DistributedObjectBroker::instance();

			DistributedObjectAdapter* adapter = orb->getObjectAdapter(objectID);
			if (adapter == NULL) {
				orb->error("object not found for method invocation");
				return;
			}

			DistributedMethod invocation(orb, this);
			adapter->invokeMethod(methodID, &invocation);

			DOBMessage* response = (DOBMessage*) invocation.getResponseMessage();
			client->sendReply(response);
		}

		void handleReply(Packet* resp) {
			packet = resp->clone();
		}

		Packet* getResponseMessage() {
			return packet;
		}

		bool getBooleanParameter() {
			return packet->parseBoolean();
		}

		byte getByteParameter() {
			return packet->parseByte();
		}

		char getSignedCharParameter() {
			return packet->parseSignedByte();
		}

		unsigned char getUnsignedCharParameter() {
			return packet->parseByte();
		}

		int getSignedIntParameter() {
			return packet->parseSignedInt();
		}

		unsigned int getUnsignedIntParameter() {
			return packet->parseInt();
		}

		long long getSignedLongParameter() {
			return packet->parseSignedLong();
		}

		unsigned long long getUnsignedLongParameter() {
			return packet->parseLong();
		}

		short getSignedShortParameter() {
			return packet->parseSignedShort();
		}

		unsigned short getUnsignedShortParameter() {
			return packet->parseShort();
		}

		float getFloatParameter() {
			return packet->parseFloat();
		}

		double getDoubleParameter() {
			return packet->parseDouble();
		}

		String& getAsciiParameter(String& ascii) {
			packet->parseAscii(ascii);

			return ascii;
		}

		UnicodeString& getUnicodeParameter(UnicodeString& str) {
			packet->parseUnicode(str);

			return str;
		}

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*INVOKEMETHODMESSAGE_H_*/
