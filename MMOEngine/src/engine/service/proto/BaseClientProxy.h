/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BASECLIENTPROXY_H_
#define BASECLIENTPROXY_H_

#include "system/lang.h"

#include "BaseClient.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClientProxy : public BaseClient {
	public:
		BaseClientProxy();
		BaseClientProxy(Socket* sock, SocketAddress& addr);

		virtual ~BaseClientProxy() {
			socket = nullptr;
		}

		void init(DatagramServiceThread* serv);

	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASECLIENTPROXY_H_*/
