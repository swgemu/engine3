/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef CONNECTIONSERVERMESSAGE_H_
#define CONNECTIONSERVERMESSAGE_H_

#include "engine/service/proto/BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class ConnectionServerMessage : public BasePacket {
	public:
		ConnectionServerMessage() : BasePacket(100) {
			insertShort(0x0900);
			insertShort(0x00);
			insertShort(0x1900);
	
			insertByte(0x1E);
			insertShort(0x02);
			insertInt(0x0E20D7E9);
			insertAscii("ConnectionServer:15453", 0x16);
			
			insertByte(0x0A);
			insertShort(0x02);
			insertInt(0x58C07F21);
			insertInt(0x3C5D);
		}
	
		/*static uint16 parse(Packet* pack) {
			return 0;
		}*/
		
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*CONNECTIONSERVERMESSAGE_H_*/
