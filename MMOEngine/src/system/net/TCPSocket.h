/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#include "Socket.h"

namespace sys {
  namespace net {

	class TCPSocket : public Socket {
	public:
	  	TCPSocket() : Socket() {
			fileDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (fileDescriptor < 0)
				throw SocketException("unable to create socket");
	  	}
	
	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*TCPSOCKET_H_*/
