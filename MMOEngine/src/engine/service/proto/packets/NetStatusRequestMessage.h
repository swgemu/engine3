/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef NETSTATUSREQUESTMESSAGE_H_
#define NETSTATUSREQUESTMESSAGE_H_

#include "engine/service/proto/BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class NetStatusRequestMessage : public BasePacket {
	public:
		NetStatusRequestMessage(sys::uint16 tick) : BasePacket(7) {
			insertShort(0x0700);
			insertShort(tick);
			
			setSequencing(false);
			setCompression(true);
		}
	
		static sys::uint16 parseTick(Packet* pack) {
			return pack->parseShort(2);
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*NETSTATUSREQUESTMESSAGE_H_*/
