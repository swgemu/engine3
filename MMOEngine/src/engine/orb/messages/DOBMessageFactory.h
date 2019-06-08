/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DOBMESSAGEFACTORY_H_
#define DOBMESSAGEFACTORY_H_

#include "system/lang.h"

#include "DOBMessage.h"

#include "engine/core/Task.h"

namespace engine {
  namespace ORB {

	class DOBMessageFactory {
	public:
		DOBMessageFactory();

		void process(DOBServiceClient* client, Packet* packet);

	private:
		DOBMessage* create(uint32 messageType, Packet* packet);

		class MessageProcessorTask : public Task {

		};
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DOBMESSAGEFACTORY_H_*/
