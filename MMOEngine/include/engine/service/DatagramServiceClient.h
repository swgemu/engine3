/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMSERVICECLIENT_H_
#define DATAGRAMSERVICECLIENT_H_

#include "../../system/lang.h"

#include "ServiceClient.h"

namespace engine {
  namespace service {

	class DatagramServiceClient : public ServiceClient {
	protected:
		bool doRun;

	public:
		DatagramServiceClient(const String& host, int port);
		DatagramServiceClient(Socket* sock, SocketAddress& addr);

		virtual ~DatagramServiceClient();
		
		void recieveMessages();

		// socket methods
		bool send(Packet* pack);

		bool read(Packet* pack);

		virtual void handleMessage(Packet* message) {
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*DATAGRAMSERVICECLIENT_H_*/
