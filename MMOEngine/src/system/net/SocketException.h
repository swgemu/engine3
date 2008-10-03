/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SOCKETEXCEPTION_H_
#define SOCKETEXCEPTION_H_

#ifndef PLATFORM_WIN
#include <errno.h>
#else
#include <winsock2.h>
#endif

#include "../lang/Exception.h"

namespace sys {
  namespace net {

	class SocketException : public sys::lang::Exception {
	public:
		SocketException() : Exception() {
			stringstream str;
			str << "Socket Exception " << " (errno " << getErrorCode() << ")";

			message = str.str();
		}

		SocketException(const string msg) : Exception(msg) {
			stringstream str;
			str << msg << " (errno " << getErrorCode() << ")";

			message = str.str();
		}

		int getErrorCode() {
			#ifndef PLATFORM_WIN
				return errno;
			#else
				return WSAGetLastError();
			#endif
		}

	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*SOCKETEXCEPTION_H_*/
