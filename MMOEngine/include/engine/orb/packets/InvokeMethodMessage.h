/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef INVOKEMETHODMESSAGE_H_
#define INVOKEMETHODMESSAGE_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class InvokeMethodMessage : public Packet {
	public:	
		InvokeMethodMessage(uint64 objectid, sys::uint32 methid, sys::uint32 invid) : Packet(40) {
			insertInt(0x05);
			
			insertLong(objectid);
			insertInt(methid);
			insertInt(invid);
		}
		
		static void parseObjectName(Packet* pack, String& name) {
			pack->parseAscii(name);
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*INVOKEMETHODMESSAGE_H_*/
