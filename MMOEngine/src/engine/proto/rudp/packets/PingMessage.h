/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PINGMESSAGE_H_
#define PINGMESSAGE_H_

#include "../BaseProtocol.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class PingMessage : public RUDPPacket {
	public:
		PingMessage() : RUDPPacket(7) {
			insertShort(0x0600);
		}
	
		static void parse(Packet* pack) {
		}
		
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*PINGMESSAGE_H_*/
