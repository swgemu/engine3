/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ERRORMESSAGE_H_
#define ERRORMESSAGE_H_

#include "engine/service/proto/BaseProtocol.h"

namespace engine {
  namespace service {
    namespace proto {

	class ErrorMessage : public BasePacket {
	public:
		ErrorMessage(const String& ErrorType, const String& ErrorMsg, bool Fatal) : BasePacket(50) {
			insertShort(0x03);
			insertAscii(ErrorType);
			insertAscii(ErrorMsg);
			insertByte(Fatal);
		}
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*ERRORMESSAGE_H_*/
