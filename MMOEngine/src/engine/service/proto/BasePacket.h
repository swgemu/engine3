/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASEPACKET_H_
#define BASEPACKET_H_

#include "engine/service/Message.h"

namespace engine {
  namespace service {
    namespace proto {

	class BasePacket : public Message {
	protected:
		sys::uint32 sequence;
		bool doSeq;
	
		bool doEncr;
		bool doComp;
		bool doCRCTest;
	
		Time timestamp;
		Time timeout;
		
		int outOfOrderCount;
	
	public:
		BasePacket();	
		BasePacket(int size);	
		BasePacket(Packet* pack, uint32 seq);
	
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
		inline sys::uint32 getSequence() {
			if (doSeq)
				return sequence;
			else
				return 0xFFFFFFFF;
		}
		
		inline bool doSequencing() {
			return doSeq;
		}
	
		inline bool doEncryption() {
			return doEncr;
		}
	
		inline bool doCompression() {
			return doComp;
		}
		
		inline bool doCRCChecking() {
			return doCRCTest;
		}
	
		inline Time& getTimestamp() {
			return timestamp;
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
