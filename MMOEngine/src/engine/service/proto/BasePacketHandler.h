/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BASEPACKETHANDLER_H_
#define BASEPACKETHANDLER_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "engine/service/ServiceClient.h"
#include "engine/service/ServiceHandler.h"

#include "BaseClient.h"

#include "BaseMessage.h"

namespace engine {
  namespace service {
    namespace proto {

	class BasePacketHandler : public Logger, public Object {
		ServiceHandler* serviceHandler;

	public:
		BasePacketHandler();
		BasePacketHandler(const String& s, ServiceHandler* handler);

		void handlePacket(BaseClient* client, Packet* pack);

		void doSessionStart(BaseClient* client, Packet* pack);
		void doSessionResponse(BaseClient* client, Packet* pack);

		void doDisconnect(BaseClient* client, Packet* pack);
		void doNetStatusResponse(BaseClient* client, Packet* pack);
		void doOutOfOrder(BaseClient* client, Packet* pack);
		void doAcknowledge(BaseClient* client, Packet* pack);

		void processBufferedPackets(BaseClient* client);

		void handleMultiPacket(BaseClient* client, Packet* pack, bool validatePackets = true);

		void handleDataChannelPacket(BaseClient* client, Packet* pack);
		void handleDataChannelMultiPacket(BaseClient* client, Packet* pack, sys::uint16 size);

		int handleFragmentedPacket(BaseClient* client, Packet* pack);

		void processMessage(Message* msg);
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASEPACKETHANDLER_H_*/

