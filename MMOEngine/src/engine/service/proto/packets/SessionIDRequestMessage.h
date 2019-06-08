/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SESSIONIDREQUESTMESSAGE_H_
#define SESSIONIDREQUESTMESSAGE_H_

#include "engine/service/proto/BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class SessionIDRequestMessage : public Packet {
	public:
		SessionIDRequestMessage() : Packet(10) {
			insertShort(0x0100);
			insertShort(0x00);
			insertShort(0x01);
			insertInt(0x12345678);
		}
	
		static void parse(Packet* pack, BaseProtocol* prot) {
			sys::uint32 connID = pack->parseInt(6);
	        prot->setConnectionID(connID);
		}

		static uint32 parse(Packet* pack) {
			return pack->parseInt(6);
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*SESSIONIDREQUESTMESSAGE_H_*/
