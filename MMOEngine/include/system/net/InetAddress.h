/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef INETADDRESS_H_
#define INETADDRESS_H_

#include "../platform.h"

#ifndef PLATFORM_WIN
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <winsock2.h>
#endif

#include "../lang/String.h"

namespace sys {
  namespace net {

	class InetAddress {
		String ipaddress;
		String hostname;

	public:
		InetAddress(const String& address);

		~InetAddress() {
		}

		// setters
		inline void setHostName(const String& host) {
			hostname = host;
		}

		// getters
		inline const String& getHostAddress() {
			return ipaddress;
		}

		inline const String& getHostName() {
			return hostname;
		}

	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*INETADDRESS_H_*/
