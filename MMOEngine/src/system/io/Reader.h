/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef READER_H_
#define READER_H_

#include "system/lang/Object.h"

namespace sys {
  namespace io {

  	class Reader : public Object {
  	public:
  		virtual ~Reader() {
  		}

  		// Closes the stream and releases any system resources associated with it.
  		virtual void close() = 0;

  		// Reads characters into an array.
  		virtual int read(char* buf, int len) = 0;

  		// Reads characters into a portion of an array.
  		virtual int read(char* buf, uint32 off, int len) = 0;

  		// Skips characters.
  		virtual int skip(int n) = 0;
  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*READER_H_*/
