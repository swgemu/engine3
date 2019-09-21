/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef PRINTSTREAM_H_
#define PRINTSTREAM_H_

#include "system/platform.h"

#include "system/lang/String.h"
#include "system/lang/StringBuffer.h"
#include "system/lang/UnicodeString.h"

namespace sys {
  namespace io {

	class PrintStream {
		typedef decltype(stdout) StreamType;

		StreamFlags streamFlags;
		StreamType outStream;

	public:
		constexpr static const char endl = '\n';

	public:
		PrintStream(StreamType type = stdout);

		void print(char ch);
		void print(int val);
		void print(uint32 val);
		void print(long val);
		void print(int64 val);
		void print(uint64 val);
		void print(float val);
		void print(void* val);
		void print(const char* str);
		void print(const char* str, int length);
		void print(const String& str);
		void print(const UnicodeString& str);

		void println(char ch);
		void println(int val);
		void println(uint32 val);
		void println(long val);
		void println(int64 val);
		void println(uint64 val);
		void println(float val);
		void println(void* val);
		void println(const char* str);
		void println(const char* str, int length);
		void println(const String& str);
		void println(const UnicodeString& str);

		void flush();

		PrintStream& operator<< (char ch);
		PrintStream& operator<< (int val);
		PrintStream& operator<< (uint32 val);
		PrintStream& operator<< (long val);
		PrintStream& operator<< (int64 val);
		PrintStream& operator<< (uint64 val);
		PrintStream& operator<< (unsigned long val);
		PrintStream& operator<< (float val);
		PrintStream& operator<< (void* val);
		PrintStream& operator<< (const char* str);
		PrintStream& operator<< (const StringBuffer& msg);
		PrintStream& operator<< (const String& str);
		PrintStream& operator<< (const UnicodeString& str);
		PrintStream& operator<< (const StreamFlags flags);

		// getters
		inline bool doHex() const {
			return streamFlags & SF_hex;
		}

		inline bool doUpperCase() const {
			return streamFlags & SF_uppercase;
		}

		inline StreamType getStreamType() const {
			return outStream;
		}

	private:
		inline const char* upf(const char* lower, const char* upper) const {
			return doUpperCase() ? upper : lower;
		}

	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*PRINTSTREAM_H_*/
