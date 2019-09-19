/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BASEMESSAGE_H_
#define BASEMESSAGE_H_

#include "engine/service/DatagramServiceClient.h"

#include "BasePacket.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseMessage : public BasePacket {
	public:
		BaseMessage() : BasePacket() {
			insertShort(0x0900);
			insertShort(0x0000);
		}

		BaseMessage(int size) : BasePacket(size) {
			insertShort(0x0900);
			insertShort(0x0000);
		}

		BaseMessage(const Packet* pack, int start) : BasePacket(pack->size() - start) {
			if (start < 0)
				throw PacketIndexOutOfBoundsException(pack, start);

			insertStream(pack->getBuffer() + start, pack->size() - start);

			reset();
		}

		BaseMessage(const Packet* pack, int startoffs, int endoffs) : BasePacket(endoffs - startoffs) {
			if (startoffs < 0)
				throw PacketIndexOutOfBoundsException(pack, startoffs);

			if (endoffs > pack->size())
				throw PacketIndexOutOfBoundsException(pack, endoffs);

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

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASEMESSAGE_H_*/
