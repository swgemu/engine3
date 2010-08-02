/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ACKNOWLEDGEOKMESSAGE_H_
#define ACKNOWLEDGEOKMESSAGE_H_

#include "../BaseProtocol.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class AcknowledgeOkMessage : public RUDPPacket {
	public:
		AcknowledgeOkMessage(sys::uint16 seq) : RUDPPacket(17) {
			insertShort(0x0300);
			insertByte(0x04);
			insertShort(0x1500);
			insertShortNet(seq);
			insertByte(0x06);
			insertShort(0x0001);
			insertInt(0xA16CF9AF);
			
			setSequencing(false);
		}
	
		inline static void parseOk(Packet* pack) {
			pack->parseInt();
		}
		
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*ACKNOWLEDGEOKMESSAGE_H_*/
