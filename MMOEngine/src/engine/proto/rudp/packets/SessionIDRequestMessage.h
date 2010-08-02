/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SESSIONIDREQUESTMESSAGE_H_
#define SESSIONIDREQUESTMESSAGE_H_

#include "../BaseProtocol.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class SessionIDRequestMessage : public Packet {
	public:
		SessionIDRequestMessage() : Packet(10) {
			insertShort(0x0100);
			insertShort(0x00);
			insertShort(0x01);
			insertInt(0x12345678);
		}
	
		static void parse(Packet* pack, BaseProtocol* prot) {
			sys::uint32 connID = pack->parseInt(6);
	        prot->setConnectionID(connID);
		}
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*SESSIONIDREQUESTMESSAGE_H_*/
