/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef RUDPPACKET_H_
#define RUDPPACKET_H_

#include "engine/service/Message.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class RUDPPacket : public Message {
	protected:
		sys::uint32 sequence;
		bool doSeq;
	
		bool doEncr;
		bool doComp;
		bool doCRCTest;
	
		Time timestamp;
		Time timeout;
	
	public:
		RUDPPacket();	
		RUDPPacket(int size);	
		RUDPPacket(Packet* pack, int seq);
	
		virtual ~RUDPPacket() {
		}
		
		void close();

		RUDPPacket* clone(int startoffs = 0);

		int compareTo(RUDPPacket* pack);

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

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*RUDPPACKET_H_*/
