#ifndef IOEXCEPTION_H_
#define IOEXCEPTION_H_

#include <errno.h>

#include "system/lang/Exception.h"

namespace sys {
  namespace io {

	class IOException : public Exception {
	public:
		IOException(const String& msg) {
			message = msg + " (" + String::valueOf(errno) + ": " + strerror(errno)  +  ")";
		}

	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*IOEXCEPTION_H_*/
