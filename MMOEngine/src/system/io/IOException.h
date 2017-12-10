#ifndef IOEXCEPTION_H_
#define IOEXCEPTION_H_

#include <errno.h>

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

#endif /*IOEXCEPTION_H_*/
