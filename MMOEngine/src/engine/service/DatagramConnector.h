/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMSERVICECLIENT_H_
#define DATAGRAMSERVICECLIENT_H_

#include "system/lang.h"

#include "ServiceSession.h"

namespace engine {
  namespace service {

	class DatagramConnector : public ServiceSession {
	protected:
		DatagramSocket* socket;

		bool doRun;

	public:
		DatagramConnector();
		DatagramConnector(const String& addr, int port);

		virtual ~DatagramConnector();
		
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
