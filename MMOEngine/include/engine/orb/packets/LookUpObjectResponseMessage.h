/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOOKUPOBJECTRESPONSEMESSAGE_H_
#define LOOKUPOBJECTRESPONSEMESSAGE_H_

#include "system/lang.h"

#include "../object/DistributedObject.h"

namespace engine {
  namespace ORB {

	class LookUpObjectResponseMessage : public Packet {
	public:	
		LookUpObjectResponseMessage(DistributedObject* obj) : Packet(40) {
			if (obj != NULL) {
				insertByte(1);
				insertAscii(obj->_getClassName());
				insertLong(obj->_getObjectID());
			} else
				insertByte(0);
		}
	
		static bool parseObjectFound(Packet* pack) {
			sys::uint8 found = pack->parseByte();
			
			if (found == 1)
				return true;
			else 
				return false;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*LOOKUPOBJECTRESPONSEMESSAGE_H_*/
