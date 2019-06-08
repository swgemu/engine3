/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef PACKETHANDLER_H_
#define PACKETHANDLER_H_

namespace engine {
  namespace service {
    namespace proto {

	class PacketHandler {
	public:
		virtual void handlePacket(Packet* pack) = 0;
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*PACKETHANDLER_H_*/
