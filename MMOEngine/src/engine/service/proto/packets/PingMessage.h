/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PINGMESSAGE_H_
#define PINGMESSAGE_H_

#include "../BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class PingMessage : public BasePacket {
	public:
		PingMessage() : BasePacket(7) {
			insertShort(0x0600);
		}
	
		static void parse(Packet* pack) {
		}
		
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*PINGMESSAGE_H_*/
