/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef STATEUPDATEMESSAGE_H_
#define STATEUPDATEMESSAGE_H_

#include "system/lang.h"

#include "engine/orb/messages/RemoteObjectBroker.h"

#include "engine/orb/messages/DOBMessage.h"

#include "ObjectBrokerDirector.h"

namespace engine {
  namespace ORB {

	class StateUpdateMessage : public DOBMessage {
		int state;

	public:
		StateUpdateMessage(int state) : DOBMessage(STATEUPDATEMESSAGE, 20), state(state) {
			insertInt(state);
		}

		StateUpdateMessage(Packet* message) : DOBMessage(message) {
			state = message->parseInt();
		}

		void execute() {
			ObjectBrokerDirector* director = ObjectBrokerDirector::instance();

			RemoteObjectBroker* remoteBroker = getClient()->getRemoteObjectBroker();

			director->handleStateUpdate(remoteBroker, state);
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*STATEUPDATEMESSAGE_H_*/
