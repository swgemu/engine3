#ifndef PRINTSTREAM_H_
#define PRINTSTREAM_H_

#include "../platform.h"

namespace sys {
  namespace io {

	class PrintStream {
	public:
		PrintStream() {
		}

		// stream manipulation methods
		void print(const char* s) {
			printf("%s", s);
		}

		void println(const char* s) {
			printf("%s\n", s);
		}
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*PRINTSTREAM_H_*/
