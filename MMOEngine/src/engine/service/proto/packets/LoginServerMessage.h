/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef LOGINSERVERMESSAGE_H_
#define LOGINSERVERMESSAGE_H_

#include "engine/service/proto/BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class LoginServerMessage : public BasePacket {
	public:
		LoginServerMessage() : BasePacket(100) {
			insertShort(0x0900);
			insertShort(0x00);
			insertShort(0x1900);
	
			insertByte(0x19);
			insertShort(0x02);
			insertInt(0x0E20D7E9);
			insertAscii("LoginServer:29411", 0x11);
			
			insertByte(0x0A);
			insertShort(0x02);
			insertInt(0x58C07F21);
			insertInt(0x72E3);
		}
	
		/*static uint16 parse(Packet* pack) {
			return 0;
		}*/
		
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*LOGINSERVERMESSAGE_H_*/
