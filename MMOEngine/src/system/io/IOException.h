/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#pragma once

#include <cerrno>

#include "system/lang/Exception.h"

namespace sys {
  namespace io {

	class IOException : public Exception {
	public:
		IOException(const String& msg) {
			char str[256];
#ifdef PLATFORM_MAC
			int val = strerror_r(errno, str, 256);

			message = msg + " (" + String::valueOf(errno) + ": " + str  +  ")";
#else
			char* val = strerror_r(errno, str, 256);

			message = msg + " (" + String::valueOf(errno) + ": " + val  +  ")";
#endif


		}

	};

  } // namespace io
} // namespace sys

using namespace sys::io;

