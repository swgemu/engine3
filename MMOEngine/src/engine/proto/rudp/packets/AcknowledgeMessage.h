/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ACKNOWLEDGEMESSAGE_H_
#define ACKNOWLEDGEMESSAGE_H_

#include "../BaseProtocol.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class AcknowledgeMessage : public RUDPPacket {
	public:
		AcknowledgeMessage(sys::uint16 seq) : RUDPPacket(7) {
			insertShort(0x1500);
			insertShortNet(seq);
			
			setSequencing(false);
		}
	
		inline static sys::uint16 parse(Packet* pack) {
			return pack->parseNetShort();
		}
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*ACKNOWLEDGEMESSAGE_H_*/
