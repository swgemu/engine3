/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef RUDPFILTER_H_
#define RUDPFILTER_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "engine/service/ServiceSession.h"
#include "engine/service/ServiceFilter.h"
#include "engine/service/AbstractServiceAcceptor.h"

#include "RUDPProtocol.h"

#include "BaseMessage.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class RUDPFilter : public ProtocolCodecFilter, public Logger {
		MessageQueue* messageQueue;

	public:
		RUDPFilter(MessageQueue* queue);
		RUDPFilter(const String& s, MessageQueue* queue);

	private:
		void handlePacket(RUDPProtocol* client, Packet* pack);

		void doSessionStart(RUDPProtocol* client, Packet* pack);
		void doSessionResponse(RUDPProtocol* client, Packet* pack);

		void doDisconnect(RUDPProtocol* client, Packet* pack);
		void doNetStatusResponse(RUDPProtocol* client, Packet* pack);
		void doOutOfOrder(RUDPProtocol* client, Packet* pack);
		void doAcknowledge(RUDPProtocol* client, Packet* pack);

		void processBufferedPackets(RUDPProtocol* client);

		void handleMultiPacket(RUDPProtocol* client, Packet* pack);

		void handleDataChannelPacket(RUDPProtocol* client, Packet* pack);
		void handleDataChannelMultiPacket(RUDPProtocol* client, Packet* pack, sys::uint16 size);

		void handleFragmentedPacket(RUDPProtocol* client, Packet* pack);
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*RUDPFILTER_H_*/

