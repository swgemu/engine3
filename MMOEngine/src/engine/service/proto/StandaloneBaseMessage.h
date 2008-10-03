/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STANDALONEBASEMESSAGE_H_
#define STANDALONEBASEMESSAGE_H_

#include "BasePacket.h"

namespace engine {
  namespace service {
    namespace proto {

	class StandaloneBaseMessage : public BasePacket {
	public:
		StandaloneBaseMessage() : BasePacket() {
			setSequencing(false);
		}
	
		StandaloneBaseMessage(int size) : BasePacket(size) {
			setSequencing(false);
		}
	
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*STANDALONEBASEMESSAGE_H_*/
