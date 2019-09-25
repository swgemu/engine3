/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef INETADDRESS_H_
#define INETADDRESS_H_

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
		inline const String& getHostAddress() const {
			return ipaddress;
		}

		inline const String& getHostName() const {
			return hostname;
		}

	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*INETADDRESS_H_*/
