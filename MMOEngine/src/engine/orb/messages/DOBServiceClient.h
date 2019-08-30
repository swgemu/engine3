/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DOBSERVICECLIENT_H_
#define DOBSERVICECLIENT_H_

#include "engine/log/Logger.h"

#include "engine/service/StreamServiceClient.h"

#include "system/util/SynchronizedHashTable.h"

#include "DOBMessage.h"

namespace engine {
  namespace ORB {

	class RemoteObjectBroker;

	class DOBServiceClient : public StreamServiceClient, public Logger {
		RemoteObjectBroker* rob;

		SynchronizedHashTable<uint32, DOBMessage*> sentMessageQueue;
		AtomicInteger sentMessageSequence;

	public:
		DOBServiceClient(Socket* sock);
		DOBServiceClient(const String& host, int port);

		~DOBServiceClient();

		void run();

		void receiveMessages();

		bool send(DOBMessage* message);

		bool sendAndAcceptReply(DOBMessage* message);
		bool sendReply(DOBMessage* message);
		bool sendWithReply(DOBMessage* message);

		bool waitForReply(DOBMessage* message);

		inline RemoteObjectBroker* getRemoteObjectBroker() {
			return rob;
		}

		DOBMessage* getQueuedMessage(uint32 sequence) {
			DOBMessage* message = sentMessageQueue.remove(sequence);
			fatal(message != nullptr) << "message is null";

			return message;
		}
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /*DOBSERVICECLIENT_H_*/
