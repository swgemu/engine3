/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SERVICEHANDLER_H_
#define SERVICEHANDLER_H_

#include "Message.h"

#include "ServiceClient.h"

namespace engine {
  namespace service {

	class ServiceHandler : public virtual Object {
	public:
		constexpr const static bool is_virtual_object = true;

		virtual ~ServiceHandler() {
		}

		virtual void initialize() = 0;

		virtual ServiceClient* createConnection(Socket* sock, SocketAddress& addr) = 0;

		virtual bool deleteConnection(ServiceClient* client) {
			return false;
		}

		virtual void handleMessage(ServiceClient* client, Packet* message) = 0;

		virtual void processMessage(Message* message) = 0;

		virtual void handleReject(Packet* message) {
		}

		virtual void messageSent(Packet* message) {
		}

		virtual bool handleError(ServiceClient* client, Exception& e) = 0;
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /* SERVICEHANDLER_H_ */
