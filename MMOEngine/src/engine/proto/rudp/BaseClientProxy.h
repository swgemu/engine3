/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASECLIENTPROXY_H_
#define BASECLIENTPROXY_H_

#include "system/lang.h"

#include "BaseClient.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class BaseClientProxy : public BaseClient {
	public:
		BaseClientProxy();
		BaseClientProxy(SocketImplementation* sock, SocketAddress& addr);

		virtual ~BaseClientProxy() {
			socket = NULL;
		}

		void init(DatagramAcceptor* serv);

	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*BASECLIENTPROXY_H_*/
