/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SOCKETEXCEPTION_H_
#define SOCKETEXCEPTION_H_

#include "system/platform.h"

#ifndef PLATFORM_WIN
#include <errno.h>
#else
#include <winsock2.h>
#include <windows.h>
#endif

#include "system/lang/Exception.h"

#include "system/lang/StringBuffer.h"

namespace sys {
  namespace net {

	class SocketException : public sys::lang::Exception {
	public:
		SocketException() : Exception() {
			StringBuffer str;
			str << "Socket Exception " << " (errno " << getErrorCode() << ")";

			message = str.toString();
		}

		SocketException(const String& msg) : Exception(msg) {
			StringBuffer str;
			str << msg << " (errno " << getErrorCode() << ")";

			message = str.toString();
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
