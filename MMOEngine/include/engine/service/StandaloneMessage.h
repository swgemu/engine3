/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STANDALONEMESSAGE_H_
#define STANDALONEMESSAGE_H_

#include "../../system/lang.h"

#include "proto/BasePacket.h"
#include "ServiceClient.h"

#include "../orb/object/ORBObject.h"

namespace engine {
  namespace service {

	class StandaloneMessage : public BasePacket, public ORBObject {
	public:
		StandaloneMessage() : BasePacket(), DistributedObject() {
			setSequencing(false);
		}
	
		StandaloneMessage(int size) : BasePacket(size), DistributedObject() {
			setSequencing(false);
		}
	
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*STANDALONEMESSAGE_H_*/
