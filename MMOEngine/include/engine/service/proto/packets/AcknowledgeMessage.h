/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ACKNOWLEDGEMESSAGE_H_
#define ACKNOWLEDGEMESSAGE_H_

#include "../BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class AcknowledgeMessage : public BasePacket {
	public:
		AcknowledgeMessage(sys::uint16 seq) : BasePacket(7) {
			insertShort(0x1500);
			insertShortNet(seq);
			
			setSequencing(false);
		}
	
		inline static sys::uint16 parse(Packet* pack) {
			return pack->parseNetShort();
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*ACKNOWLEDGEMESSAGE_H_*/
