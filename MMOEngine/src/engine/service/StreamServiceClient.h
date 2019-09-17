/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef STREAMSERVICECLIENT_H_
#define STREAMSERVICECLIENT_H_

#include "system/lang.h"

#include "ServiceClient.h"

namespace engine {
  namespace service {

	class StreamServiceClient : public ServiceClient, public Thread {
	protected:
		bool doRun;

	public:
		StreamServiceClient(Socket* sock);
		StreamServiceClient(Socket* sock, const SocketAddress& addr);
		StreamServiceClient(const String& host, int port);

		virtual ~StreamServiceClient();

		void connect();

		void start();

		void run();

		void stop();

		// message methods
		virtual void receiveMessages();

		// socket methods
		int send(Packet* pack);

		bool read(Packet* pack);

		bool recieve(Packet* pack);

		bool receiveAppend(Stream* stream);

		void disconnect();

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*STREAMSERVICECLIENT_H_*/
