/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMSERVICECLIENTPROXY_H_
#define DATAGRAMSERVICECLIENTPROXY_H_

#include "system/lang.h"

#include "ServiceSession.h"

namespace engine {
  namespace service {

	class DatagramSession : public ServiceSession {
		DatagramServerSocket* socket;

	public:
		DatagramSession();

		virtual ~DatagramSession();

		void bind(const SocketAddress& address);

		void run();

		bool send(Packet* pack);

		bool read(Packet* pack);
		
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*DATAGRAMSERVICECLIENTPROXY_H_*/
