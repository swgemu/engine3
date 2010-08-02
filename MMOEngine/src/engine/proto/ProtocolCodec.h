/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PROTOCOLCODEC_H_
#define PROTOCOLCODEC_H_

#include "system/lang.h"

namespace engine {
  namespace proto {

	class ProtocolCodec {
	public:
		virtual void encode(Packet* message) = 0;

		virtual void decode(Packet* message) = 0;
	};

  } // namespace proto
} // namespace engine

using namespace engine::proto;

#endif /* PROTOCOLCODEC_H_ */
