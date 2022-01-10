/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef UPDATEMODIFIEDOBJECTSMESSAGE_H_
#define UPDATEMODIFIEDOBJECTSMESSAGE_H_

#include "system/lang.h"

#include "engine/orb/messages/DOBMessage.h"

#include "ObjectBrokerAgent.h"

namespace engine {
  namespace ORB {

	class ControlMessage : public DOBMessage {
		int command;
		int flags;

	public:
		ControlMessage(int command, int flags) : DOBMessage(CONTROLMESSAGE, 20), command(command) {
			insertInt(command);
			insertInt(flags);
		}

		ControlMessage(Packet* message) : DOBMessage(message) {
			command = message->parseInt();
			flags = message->parseInt();
		}

		void execute() {
			ObjectBrokerAgent* agent = ObjectBrokerAgent::instance();

			agent->doCommand((ObjectBrokerDirector::Command) command, flags);
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*UPDATEMODIFIEDOBJECTSMESSAGE_H_*/
