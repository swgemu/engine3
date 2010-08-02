/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DATAGRAMSOCKET_H_
#define DATAGRAMSOCKET_H_

#include "SocketImplementation.h"

namespace sys {
  namespace net {

	class DatagramSocket {
		SocketImplementation* socketImpl;

		SocketAddress address;

	public:
		DatagramSocket() {
			socketImpl = new SocketImplementation(SocketImplementation::DATAGRAM_SOCKET);
		}

		virtual ~DatagramSocket() {
			close();
		}

		inline void bind(const String& ip, int port) {
			address = SocketAddress(ip, port);
		}

		inline void bind(const SocketAddress& addr) {
			address = addr;
		}

		inline void close() {
			if (socketImpl != NULL) {
				socketImpl->close();

				delete socketImpl;
				socketImpl = NULL;
			}
		}

		inline void send(Packet* packet) {
			socketImpl->sendTo(packet, address);
		}

		inline bool read(Packet* packet) {
			return socketImpl->read(packet);
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

#endif /*DATAGRAMSOCKET_H_*/
