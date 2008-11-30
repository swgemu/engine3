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
	
		static void parseObjectName(Packet* pack, String& name) {
			pack->parseAscii(name);
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*LOOKUPOBJECTMESSAGE_H_*/
