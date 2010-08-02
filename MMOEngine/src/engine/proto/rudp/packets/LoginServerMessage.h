/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOGINSERVERMESSAGE_H_
#define LOGINSERVERMESSAGE_H_

#include "../BaseProtocol.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class LoginServerMessage : public RUDPPacket {
	public:
		LoginServerMessage() : RUDPPacket(100) {
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

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*LOGINSERVERMESSAGE_H_*/
