/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PACKETINDEXOUTOFBOUNDSEXCEPTION_H_
#define PACKETINDEXOUTOFBOUNDSEXCEPTION_H_

#include "../lang/Exception.h"

#include "../lang/StringBuffer.h"

namespace sys {
  namespace net {

	class Packet;

	class PacketIndexOutOfBoundsException : public Exception {
		Packet* packet;

	public:
		PacketIndexOutOfBoundsException(Packet* pack, int index) : Exception() {
			packet = pack;

			StringBuffer str;
			str << "PacketIndexOutOfBoundsException at " << index << "\n";
			message = str.toString();
		}

		Packet* getPacket() {
			return packet;
		}

	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*PACKETINDEXOUTOFBOUNDSEXCEPTION_H_*/
