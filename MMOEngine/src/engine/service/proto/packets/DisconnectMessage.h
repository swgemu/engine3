/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DISCONNECTMESSAGE_H_
#define DISCONNECTMESSAGE_H_

#include "engine/service/proto/BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class DisconnectMessage : public BasePacket {
	public:
		DisconnectMessage(BaseProtocol* proto) : BasePacket(7) {
			insertShort(0x0500);
			insertInt(proto->getConnectionID());
			insertShort(0x0600);
			
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

#endif /*DISCONNECTMESSAGE_H_*/
