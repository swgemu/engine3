/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/lang.h"

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

