/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ACKNOWLEDGEMESSAGE_H_
#define ACKNOWLEDGEMESSAGE_H_

#include "engine/service/proto/BaseProtocol.h"

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
