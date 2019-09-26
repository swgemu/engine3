/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/platform.h"

#include "system/lang/String.h"
#include "system/lang/StringBuffer.h"
#include "system/lang/UnicodeString.h"

namespace sys {
  namespace io {
	class File;

	class TextStream {
		typedef decltype(stdout) StreamType;

		StreamFlags streamFlags;
		StreamType outStream;

	public:
		constexpr static const char endl = '\n';

	public:
		TextStream(StreamType type = stdout);
		TextStream(File& file);

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

		TextStream& operator<< (char ch);
		TextStream& operator<< (int val);
		TextStream& operator<< (uint32 val);
		TextStream& operator<< (long val);
		TextStream& operator<< (int64 val);
		TextStream& operator<< (uint64 val);
		TextStream& operator<< (unsigned long val);
		TextStream& operator<< (float val);
		TextStream& operator<< (void* val);
		TextStream& operator<< (const char* str);
		TextStream& operator<< (const StringBuffer& msg);
		TextStream& operator<< (const String& str);
		TextStream& operator<< (const UnicodeString& str);
		TextStream& operator<< (const StreamFlags flags);

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

