/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DEPLOYOBJECTMESSAGE_H_
#define DEPLOYOBJECTMESSAGE_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class DeployObjectMessage : public Packet {
	public:	
		DeployObjectMessage(const String& name, const String& classname) : Packet(40) {
			insertInt(0x03);
			insertAscii(name);
			insertAscii(classname);
		}
	
		static void parseObjectName(Packet* pack, String& name) {
			pack->parseAscii(name);
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DEPLOYOBJECTMESSAGE_H_*/
