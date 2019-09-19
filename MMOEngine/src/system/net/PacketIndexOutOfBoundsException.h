/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef PACKETINDEXOUTOFBOUNDSEXCEPTION_H_
#define PACKETINDEXOUTOFBOUNDSEXCEPTION_H_

#include "system/lang/Exception.h"

#include "system/lang/StringBuffer.h"

namespace sys {
  namespace net {

	class Packet;

	class PacketIndexOutOfBoundsException : public Exception {
		const Packet* packet;

	public:
		PacketIndexOutOfBoundsException(const Packet* pack, int index) : Exception() {
			packet = pack;

			StringBuffer str;
			str << "PacketIndexOutOfBoundsException at " << index << "\n";
			message = str.toString();
		}

		const Packet* getPacket() const {
			return packet;
		}

	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*PACKETINDEXOUTOFBOUNDSEXCEPTION_H_*/
