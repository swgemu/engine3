/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STANDALONEBASEMESSAGE_H_
#define STANDALONEBASEMESSAGE_H_

#include "RUDPPacket.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class StandaloneBaseMessage : public RUDPPacket {
	public:
		StandaloneBaseMessage() : RUDPPacket() {
			setSequencing(false);
		}
	
		StandaloneBaseMessage(int size) : RUDPPacket(size) {
			setSequencing(false);
		}
	
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*STANDALONEBASEMESSAGE_H_*/
