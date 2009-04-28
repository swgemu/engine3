/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OUTPUTSTREAM_H_
#define OUTPUTSTREAM_H_

namespace sys {
  namespace io {

  	class OutputStream {
  	public:
  		virtual ~OutputStream() {
  		}

  		// Closes this output stream and releases any system resources associated with this stream.
  		virtual void close() = 0;

  		// Flushes this output stream and forces any buffered output bytes to be written out.
  		virtual void flush() = 0;

  		// Writes len bytes from the specified byte array to this output stream.
  		virtual int write(byte* buf, int len) = 0;

  		// Writes len bytes from the specified byte array starting at offset off to this output stream.
  		virtual int write(byte* buf, uint32 off, int len) = 0;

  	};
  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*OUTPUTSTREAM_H_*/
