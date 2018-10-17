/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STREAMSERVICECLIENTPROXY_H_
#define STREAMSERVICECLIENTPROXY_H_

#include "StreamServiceClient.h"

namespace engine {
  namespace service {

	class StreamServiceClientProxy : public StreamServiceClient {
	public:
		StreamServiceClientProxy(Socket* sock) : StreamServiceClient(sock) {
			doRun = true;
		}

		StreamServiceClientProxy(Socket* sock, SocketAddress& addr) : StreamServiceClient(sock, addr) {
			doRun = true;
		}

		virtual ~StreamServiceClientProxy() {
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*STREAMSERVICECLIENTPROXY_H_*/
