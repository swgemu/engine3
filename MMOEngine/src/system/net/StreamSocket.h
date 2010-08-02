/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STREAMSOCKET_H_
#define STREAMSOCKET_H_

#include "SocketImplementation.h"

namespace sys {
  namespace net {

  	class StreamSocket {
		SocketImplementation* socketImpl;

		SocketAddress address;

		static const int DEFAULT_TIMEOUT = 10;

	public:
		StreamSocket() {
			socketImpl = new SocketImplementation(SocketImplementation::STREAM_SOCKET);
			socketImpl->setTimeOut(DEFAULT_TIMEOUT);
		}

		StreamSocket(SocketImplementation* socket) {
			socketImpl = socket;
		}

		virtual ~StreamSocket() {
			close();
		}

		inline void connect(const String& ip, int port) {
			address = SocketAddress(ip, port);

			socketImpl->connect(address);
		}

		inline void connect(const SocketAddress& addr) {
			address = addr;

			socketImpl->connect(address);
		}

		inline void send(Packet* packet) {
			socketImpl->send(packet);
		}

		inline bool read(Packet* packet) {
			return socketImpl->read(packet);
		}

		inline void close() {
			if (socketImpl != NULL) {
				socketImpl->close();

				delete socketImpl;
				socketImpl = NULL;
			}
		}
	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*STREAMSOCKET_H_*/
