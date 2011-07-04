/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASESTANDALONEMESSAGE_H_
#define BASESTANDALONEMESSAGE_H_

#include "BasePacket.h"

#include "../../orb/object/ORBObject.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseStandaloneMessage : public BasePacket, public ORBObject {
	public:
		BaseStandaloneMessage() : BasePacket(), DistributedObject() {
			setSequencing(false);
		}
	
		BaseStandaloneMessage(int size) : BasePacket(size), DistributedObject() {
			setSequencing(false);
		}
	
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASESTANDALONEMESSAGE_H_*/
