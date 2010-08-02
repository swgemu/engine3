/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASEMESSAGE_H_
#define BASEMESSAGE_H_

#include "engine/service/DatagramConnector.h"

#include "RUDPPacket.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class BaseMessage : public RUDPPacket {
	public:
		BaseMessage() : RUDPPacket() {
			insertShort(0x0900);
			insertShort(0x0000);
		}
	
		BaseMessage(int size) : RUDPPacket(size) {
			insertShort(0x0900);
			insertShort(0x0000);
		}

		BaseMessage(Packet* pack, int start) : RUDPPacket(pack->size() - start) {
			if (start < 0)
				throw new PacketIndexOutOfBoundsException(pack, start);
	
			insertStream(pack->getBuffer() + start, pack->size() - start);

			reset();
		}
	
		BaseMessage(Packet* pack, int startoffs, int endoffs) : RUDPPacket(endoffs - startoffs) {
			if (startoffs < 0)
				throw new PacketIndexOutOfBoundsException(pack, startoffs);
	
			if (endoffs > pack->size())
				throw new PacketIndexOutOfBoundsException(pack, endoffs);
			
			insertStream(pack->getBuffer() + startoffs, endoffs - startoffs);
			
			reset();
		}

		virtual ~BaseMessage() {
		}
		
		BaseMessage* clone(int startoffs = 0) {
			BaseMessage* pack = new BaseMessage();
			copy(pack, startoffs);
			
			pack->doSeq = doSeq;
			pack->doEncr = doEncr;
			pack->doComp = doComp;
			pack->doCRCTest = doCRCTest;
			return pack;
		}

	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*BASEMESSAGE_H_*/
