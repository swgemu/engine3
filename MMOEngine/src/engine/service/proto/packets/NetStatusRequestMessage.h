/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef NETSTATUSREQUESTMESSAGE_H_
#define NETSTATUSREQUESTMESSAGE_H_

#include "../BaseProtocol.h"

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
