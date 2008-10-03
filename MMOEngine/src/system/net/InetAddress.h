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

namespace sys {
  namespace net {

	class InetAddress {
		string ipaddress;
		string hostname;

	public:
		InetAddress(const string& address);
		
		~InetAddress() {
		}
		
		// setters
		inline void setHostName(const string& host) {
			hostname = host;
		}

		// getters
		inline const string& getHostAddress() {
			return ipaddress;
		}

		inline const string& getHostName() {
			return hostname;
		}

	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*INETADDRESS_H_*/
