/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DATAGRAMSERVICECLIENTPROXY_H_
#define DATAGRAMSERVICECLIENTPROXY_H_

#include "system/lang.h"

#include "ServiceClient.h"

namespace engine {
  namespace service {

	class DatagramServiceClientProxy : public ServiceClient {
	public:
		DatagramServiceClientProxy(Socket* sock, SocketAddress& addr);

		virtual ~DatagramServiceClientProxy();
		
		void run();

		int send(Packet* pack);

		bool read(Packet* pack);
		
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*DATAGRAMSERVICECLIENTPROXY_H_*/
