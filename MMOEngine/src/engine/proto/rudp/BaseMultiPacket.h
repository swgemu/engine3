/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASEMULTIPACKET_H_
#define BASEMULTIPACKET_H_

#include "RUDPPacket.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class BaseMultiPacket : public RUDPPacket {
		RUDPPacket* singlePacket;
		
	public:
		BaseMultiPacket(RUDPPacket* pack);
	
		~BaseMultiPacket();
		
		bool add(RUDPPacket* pack);

		RUDPPacket* getPacket();
		
	protected:
		void insertPacket(RUDPPacket* pack);
		
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*BASEMULTIPACKET_H_*/
