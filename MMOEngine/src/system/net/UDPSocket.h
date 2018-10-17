/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

#include "Socket.h"

namespace sys {
  namespace net {

	class UDPSocket : public Socket {
	public:
		UDPSocket() : Socket() {
			fileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			
			if (fileDescriptor < 0)
				throw SocketException("unable to create socket");
		}
		
	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*UDPSOCKET_H_*/
