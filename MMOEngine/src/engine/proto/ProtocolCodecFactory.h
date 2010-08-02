/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PROTOCOLCODECFACTORY_H_
#define PROTOCOLCODECFACTORY_H_

#include "engine/service/ServiceSession.h"

#include "ProtocolCodec.h"

namespace engine {
  namespace proto {

	class ProtocolCodecFactory {
	public:
		ProtocolCodec* getCodec(ServiceSession* session);
	};

  } // namespace proto
} // namespace engine

using namespace engine::proto;

#endif /* PROTOCOLCODECFACTORY_H_ */
