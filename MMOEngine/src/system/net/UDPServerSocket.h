/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef UDPSERVERSOCKET_H_
#define UDPSERVERSOCKET_H_

#include "Socket.h"

namespace sys {
  namespace net {

	class UDPServerSocket : public Socket {
	public:
		UDPServerSocket(SocketAddress* addr) : Socket() {
			fileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	
			if (fileDescriptor < 0)
				throw SocketException("unable to create server socket");
	
			bindTo(addr);
		}
		
	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*UDPSERVERSOCKET_H_*/
