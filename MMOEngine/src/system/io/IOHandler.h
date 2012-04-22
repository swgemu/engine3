/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef IOHANDLER_H_
#define IOHANDLER_H_

#include "IOEvent.h"

namespace sys {
  namespace io {

  	class IOHandler {
  	public:
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

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /* IOHANDLER_H_ */
