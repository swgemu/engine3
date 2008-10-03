/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef METHODRETURNMESSAGE_H_
#define METHODRETURNMESSAGE_H_

#include "system/lang.h"

namespace engine {
  namespace ORB {

	class MethodReturnMessage : public Packet {
	public:	
		MethodReturnMessage(sys::uint32 invid) : Packet(40) {
			insertInt(0x06);
			
			insertInt(invid);
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*METHODRETURNMESSAGE_H_*/
