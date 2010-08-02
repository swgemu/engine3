/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STREAMSERVERSOCKET_H_
#define STREAMSERVERSOCKET_H_

#include "SocketImplementation.h"

#include "StreamSocket.h"

namespace sys {
  namespace net {

	class StreamServerSocket {
		SocketImplementation* socketImpl;

		static const int DEFAULT_TIMEOUT = 10;

	public:
		StreamServerSocket(int port) {
			socketImpl = new SocketImplementation(SocketImplementation::STREAM_SOCKET);
			socketImpl->setTimeOut(DEFAULT_TIMEOUT);

			SocketAddress addr(port);

			socketImpl->bindTo(addr);
		}

		StreamServerSocket(const SocketAddress& addr) {
			socketImpl = new SocketImplementation(SocketImplementation::STREAM_SOCKET);
			socketImpl->setTimeOut(DEFAULT_TIMEOUT);

			socketImpl->bindTo(addr);
		}

		virtual ~StreamServerSocket() {
			close();
		}

		inline SocketImplementation* accept(const SocketAddress& addr) {
			return socketImpl->accept(addr);
		}

		inline void bind(const SocketAddress& addr) {
			socketImpl->bindTo(addr);
		}

		inline void listen(int limit) {
			socketImpl->listen(limit);
		}

		inline void close() {
			if (socketImpl != NULL) {
				socketImpl->close();

				delete socketImpl;
				socketImpl = NULL;
			}
		}

		inline void setBlocking(bool status) {
			socketImpl->setBlocking(status);
		}

		inline void setLingering(int time) {
			socketImpl->setLingering(time);
		}

		inline void disableLingering() {
			socketImpl->disableLingering();
		}

		inline void setTimeOut(uint64 time) {
			socketImpl->setTimeOut(time);
		}
	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*STREAMSERVERSOCKET_H_*/
