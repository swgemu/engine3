/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STREAMINDEXOUTOFBOUNDSEXCEPTION_H_
#define STREAMINDEXOUTOFBOUNDSEXCEPTION_H_

#include "system/lang/Exception.h"

#include "system/lang/System.h"
#include "system/lang/StringBuffer.h"

namespace sys {
  namespace io {

	class StreamIndexOutOfBoundsException : public Exception {
		class Stream* stream;

	public:
		StreamIndexOutOfBoundsException(Stream* strm, int index) : Exception() {
			stream = strm;

			StringBuffer str;
			str << "StreamIndexOutOfBoundsException at " << index << "\n";
			message = str.toString();
			//System::out << message;
		}

		Stream* getStream() const {
			return stream;
		}

	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*STREAMINDEXOUTOFBOUNDSEXCEPTION_H_*/
