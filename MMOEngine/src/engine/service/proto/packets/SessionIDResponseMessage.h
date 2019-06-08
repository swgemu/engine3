/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SESSIONIDRESPONSEMESSAGE_H_
#define SESSIONIDRESPONSEMESSAGE_H_

#include "engine/service/proto/BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class SessionIDResponseMessage : public Packet {
	public:
		SessionIDResponseMessage(BaseProtocol* prot) : Packet(17) {
			insertShort(0x0200);
			insertInt(prot->getConnectionID());
			insertInt(htonl(prot->getSeed()));
			insertByte(0x02);
			insertByte(0x01); //1
			insertByte(0x04); //4
			insertByte(0x00);
			insertByte(0x00);
			insertByte(0x01);
			insertByte(0xF0);
		}
	
		static unsigned int parse(Packet* pack) {
			return ntohl(pack->parseInt(6));  // return recieved crcSeed
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*SESSIONIDRESPONSEMESSAGE_H_*/
