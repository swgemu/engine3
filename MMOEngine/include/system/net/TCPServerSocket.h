/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef TCPSERVERSOCKET_H_
#define TCPSERVERSOCKET_H_

#include "Socket.h"

namespace sys {
  namespace net {

	class TCPServerSocket : public Socket {
	public:
		TCPServerSocket(SocketAddress* addr) : Socket() {
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
			if (sock < 0)
				throw SocketException("unable to create server socket");
	
			bindTo(addr);
			
			setTimeOut(10);
		}
		
	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*TCPSERVERSOCKET_H_*/
