/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef INPUTSTREAM_H_
#define INPUTSTREAM_H_

namespace sys {
  namespace io {

  	class InputStream {
  	public:
  		virtual ~InputStream() {
  		}

  		// Closes this input stream and releases any system resources associated with the stream.
  		virtual void close() = 0;

  		// Reads up to len bytes of data from the input stream into an array of bytes.
  		virtual int read(byte* buf, int len) = 0;

  		// Reads up to len bytes of data from the input stream into an array of bytes starting at offset off.
  		virtual int read(byte* buf, uint32 off, int len) = 0;

  		// Skips over and discards n bytes of data from this input stream.
  		virtual int skip(int n) = 0;
  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*INPUTSTREAM_H_*/
