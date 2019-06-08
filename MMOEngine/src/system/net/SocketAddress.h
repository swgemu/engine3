/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SOCKETADDRESS_H_
#define SOCKETADDRESS_H_

#include "system/platform.h"

#ifndef PLATFORM_WIN
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#endif

#include "system/lang/String.h"

namespace sys {
  namespace net {

	class SocketAddress {
		struct sockaddr_in addr;

	public:
		SocketAddress();
		SocketAddress(int port);
		SocketAddress(const String& host, int port);
		SocketAddress(const SocketAddress& add);

		SocketAddress& operator=(const SocketAddress& add);

		void clear();

		int compareTo(SocketAddress* addr);

		struct sockaddr* getAddress();

	    String getIPAddress();

	    uint16 getPort();

	    String getFullIPAddress();

	    String getFullPrintableIPAddress();

		uint64 getNetworkID();

		uint32 getIPID();

		inline int getAddressSize() {
			return sizeof(addr);
		}

	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*SOCKETADDRESS_H_*/
