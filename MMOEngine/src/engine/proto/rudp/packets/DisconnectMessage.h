/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DISCONNECTMESSAGE_H_
#define DISCONNECTMESSAGE_H_

#include "../BaseProtocol.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class DisconnectMessage : public RUDPPacket {
	public:
		DisconnectMessage(BaseProtocol* proto) : RUDPPacket(7) {
			insertShort(0x0500);
			insertInt(proto->getConnectionID());
			insertShort(0x0600);
			
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

#endif /*DISCONNECTMESSAGE_H_*/
