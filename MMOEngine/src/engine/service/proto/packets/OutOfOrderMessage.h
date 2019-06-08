/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef OUTOFORDERMESSAGE_H_
#define OUTOFORDERMESSAGE_H_

namespace engine {
  namespace service {
    namespace proto {

	class OutOfOrderMessage : public BasePacket {
	public:
		OutOfOrderMessage(sys::uint16 seq) : BasePacket(7) {
			insertShort(0x1100);
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

#endif /*OUTOFORDERMESSAGE_H_*/
