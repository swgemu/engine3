/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ERRORMESSAGE_H_
#define ERRORMESSAGE_H_

#include "../BaseProtocol.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class ErrorMessage : public RUDPPacket {
	public:
		ErrorMessage(const String& ErrorType, const String& ErrorMsg, bool Fatal) : RUDPPacket(50) {
			insertShort(0x03);
			insertAscii(ErrorType);
			insertAscii(ErrorMsg);
			insertByte(Fatal);
		}
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*ERRORMESSAGE_H_*/
