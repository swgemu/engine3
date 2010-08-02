/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PROTOCOLCODECFILTER_H_
#define PROTOCOLCODECFILTER_H_

#include "system/lang.h"

#include "engine/service/ServiceFilter.h"

#include "ProtocolCodecFactory.h"

namespace engine {
  namespace proto {

	class ProtocolCodecFilter : public ServiceFilter {
		ProtocolCodecFactory* codecFactory;

	public:
		ProtocolCodecFilter(ProtocolCodecFactory* factory) {
			codecFactory = factory;
		}

		void messageSent(ServiceSession* session, Packet* message) {
			ProtocolCodec* codec = codecFactory->getCodec(session);
			codec->encode(message);
		}

		void messageReceived(ServiceSession* session, Packet* message) {
			ProtocolCodec* codec = codecFactory->getCodec(session);
			codec->decode(message);
		}

	};

  } // namespace proto
} // namespace engine

using namespace engine::proto;

#endif /* PROTOCOLCODECFILTER_H_ */
