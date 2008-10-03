/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DEPLOYOBJECTRESPONSEMESSAGE_H_
#define DEPLOYOBJECTRESPONSEMESSAGE_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class DeployObjectResponseMessage : public Packet {
	public:	
		DeployObjectResponseMessage(uint64 objid) : Packet(20) {
			insertInt(0x04);
			insertLong(objid);
		}
	
		static uint64 parseObjectID(Packet* pack) {
			return pack->parseLong();
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DEPLOYOBJECTRESPONSEMESSAGE_H_*/
