/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OUTOFORDERMESSAGE_H_
#define OUTOFORDERMESSAGE_H_

namespace engine {
  namespace proto {
    namespace rudp {

	class OutOfOrderMessage : public RUDPPacket {
	public:
		OutOfOrderMessage(sys::uint16 seq) : RUDPPacket(7) {
			insertShort(0x1100);
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

#endif /*OUTOFORDERMESSAGE_H_*/
