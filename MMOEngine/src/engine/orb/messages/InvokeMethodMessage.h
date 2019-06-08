/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef INVOKEMETHODMESSAGE_H_
#define INVOKEMETHODMESSAGE_H_

#include "system/lang.h"

#include "DOBMessage.h"

#include "engine/core/Task.h"

namespace engine {
  namespace ORB {

	class InvokeMethodMessage : public DOBMessage {
		uint64 objectID;
		uint32 methodID;
		uint32 invocationID;

		Packet* packet;

	public:	
		InvokeMethodMessage(uint64 objectid, sys::uint32 methid, sys::uint32 invid, bool async);
		InvokeMethodMessage(Packet* message);

		~InvokeMethodMessage();

		void runMethod();

		void execute();

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

		Packet* getIncomingPacket() {
			return packet;
		}

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*INVOKEMETHODMESSAGE_H_*/
