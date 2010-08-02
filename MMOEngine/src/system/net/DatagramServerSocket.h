/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMSERVERSOCKET_H_
#define DATAGRAMSERVERSOCKET_H_

#include "SocketImplementation.h"

namespace sys {
  namespace net {

	class DatagramServerSocket {
		SocketImplementation* socketImpl;

		static const int DEFAULT_TIMEOUT = 10000;

	public:
		DatagramServerSocket(int port) {
			socketImpl = new SocketImplementation(SocketImplementation::DATAGRAM_SOCKET);
			socketImpl->setTimeOut(DEFAULT_TIMEOUT);

			SocketAddress address(port);

			socketImpl->bindTo(address);
		}

		DatagramServerSocket(const SocketAddress& addr) {
			socketImpl = new SocketImplementation(SocketImplementation::DATAGRAM_SOCKET);
			socketImpl->setTimeOut(DEFAULT_TIMEOUT);

			socketImpl->bindTo(addr);
		}

		virtual ~DatagramServerSocket() {
			close();
		}

		inline void bind(const SocketAddress& addr) {
			socketImpl->bindTo(addr);
		}

		inline void close() {
			if (socketImpl != NULL) {
				socketImpl->close();

				delete socketImpl;
				socketImpl = NULL;
			}
		}

		inline void send(Packet* packet, const SocketAddress& addr) {
			socketImpl->sendTo(packet, addr);
		}

		inline bool read(Packet* packet, SocketAddress* addr) {
			if (socketImpl->hasData()) {
				socketImpl->readFrom(packet, addr);

				return true;
			} else
				return false;
		}

		inline void setBlocking() {
			socketImpl->setBlocking(true);
		}

		inline void setTimeOut(uint64 time) {
			socketImpl->setTimeOut(time);
		}

		inline SocketImplementation* getImplementation() {
			return socketImpl;
		}
	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /* DATAGRAMSERVERSOCKET_H_ */
