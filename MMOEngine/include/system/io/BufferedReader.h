#ifndef BUFFEREDREADER_H_
#define BUFFEREDREADER_H_

#include "../platform.h"

#include "Reader.h"

namespace sys {
  namespace io {

  	class BufferedReader : public Reader {
  		Reader* reader;

  		byte* buffer;

  		static const int defaultBufferSize = 8129;

  	public:
  		BufferedReader(Reader* reader);

  		void close();

  		int read(char* buf, int len);

  		int read(char* buf, uint32 off, int len);

  		int skip(int n);
  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*BUFFEREDREADER_H_*/
