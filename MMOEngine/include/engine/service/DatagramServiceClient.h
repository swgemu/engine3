/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMSERVICECLIENT_H_
#define DATAGRAMSERVICECLIENT_H_

#include "system/lang.h"

#include "ServiceClient.h"
#include "ServiceHandler.h"

namespace engine {
  namespace service {

	class DatagramServiceClient : public ServiceClient {
	protected:
		ServiceHandler* serviceHandler;

		bool doRun;

	public:
		DatagramServiceClient();
		DatagramServiceClient(const String& host, int port);
		DatagramServiceClient(Socket* sock, SocketAddress& addr);

		virtual ~DatagramServiceClient();
		
		void stop() {
			doRun = false;
		}

		void recieveMessages();

		void handleMessage(Packet* message) {
			serviceHandler->handleMessage(this, message);
		}

		// socket methods
		bool send(Packet* pack);

		bool read(Packet* pack);

		void setHandler(ServiceHandler* handler) {
			serviceHandler = handler;
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*DATAGRAMSERVICECLIENT_H_*/
