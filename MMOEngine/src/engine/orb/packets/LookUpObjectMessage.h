/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LOOKUPOBJECTMESSAGE_H_
#define LOOKUPOBJECTMESSAGE_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class LookUpObjectMessage : public Packet {
	public:	
		LookUpObjectMessage(const String& name) : Packet(40) {
			insertInt(0x01);
			insertAscii(name);
		}

		LookUpObjectMessage(uint64 objectid) : Packet(20) {
			insertInt(0x02);
			insertLong(objectid);
		}
	
		static void parseObjectName(Packet* pack, String& name) {
			pack->parseAscii(name);
		}

		static uint64 parseObjectID(Packet* pack) {
			return pack->parseLong();
		}

	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*LOOKUPOBJECTMESSAGE_H_*/
