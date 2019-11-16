/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BASEPACKET_H_
#define BASEPACKET_H_

#include "engine/service/Message.h"

namespace engine {
  namespace service {
    namespace proto {

	class BasePacket : public Message {
	protected:
		sys::uint32 sequence = 0;
		bool doSeq = true;

		bool doEncr = true;
		bool doComp = false;
		bool doCRCTest = true;

		Time timestamp;
		Time timeout;

		int outOfOrderCount;

	public:
		BasePacket();
		BasePacket(int size);
		BasePacket(const Packet* pack, uint32 seq);

		virtual ~BasePacket() {
		}

		void close();

		BasePacket* clone(int startoffs = 0);

		int compareTo(BasePacket* pack);

		bool isDataChannelPacket() {
			if (parseShort(0) == 0x0900)
				return true;
			else
				return false;
		}

		bool isMultiPacket() {
			if (parseShort(0) == 0x0300 || parseShort(4) == 0x1900)
				return true;
			else
				return false;
		}

		// setters
		inline void setSequence(sys::uint32 seq) {
			insertShort(2, htons((sys::uint16) (sequence = seq)));
		}

		inline void setSequencing(bool seq) {
			doSeq = seq;
		}

		inline void setEncryption(bool enc) {
			doEncr = enc;
		}

		inline void setCompression(bool comp) {
			doComp = comp;
		}

		inline void setCRCChecking(bool crctest) {
			doCRCTest = crctest;
		}

		inline void setTimestamp() {
			timestamp.updateToCurrentTime();
		}

		inline void setTimeout(uint64 time) {
			timeout.updateToCurrentTime();
			timeout.addMiliTime(time);
		}

		// getters
		inline sys::uint32 getSequence() const {
			if (doSeq)
				return sequence;
			else
				return 0xFFFFFFFF;
		}

		inline bool doSequencing() const {
			return doSeq;
		}

		inline bool doEncryption() const {
			return doEncr;
		}

		inline bool doCompression() const {
			return doComp;
		}

		inline bool doCRCChecking() const {
			return doCRCTest;
		}

		inline Time& getTimestamp() {
			return timestamp;
		}

		inline const Time& getTimestamp() const {
			return timestamp;
		}

		inline const Time& getTimeout() const {
			return timeout;
		}

		inline Time& getTimeout() {
			return timeout;
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASEPACKET_H_*/
