/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STREAMSERVICECLIENTPROXY_H_
#define STREAMSERVICECLIENTPROXY_H_

#include "ServiceSession.h"

namespace engine {
  namespace service {

	class StreamSession : public ServiceSession {
		StreamSocket* socket;

	public:
		StreamSession() : ServiceSession() {
			socket = NULL;
		}

		void bind(SocketImplementation* sock, const SocketAddress& address) {
			socket = new StreamSocket(sock);

			bind(sock, address);
		}

		virtual ~StreamSession() {
		}

	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*STREAMSERVICECLIENTPROXY_H_*/
