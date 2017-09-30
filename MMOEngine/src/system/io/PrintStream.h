/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PRINTSTREAM_H_
#define PRINTSTREAM_H_

#include "system/platform.h"

#include "system/lang/String.h"
#include "system/lang/StringBuffer.h"

namespace sys {
  namespace io {

	class PrintStream {
		StreamFlags streamFlags;

	public:
		static const char endl = '\n';

	public:
		PrintStream();

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

		PrintStream& operator<< (char ch);
		PrintStream& operator<< (int val);
		PrintStream& operator<< (uint32 val);
		PrintStream& operator<< (long val);
		PrintStream& operator<< (int64 val);
		PrintStream& operator<< (uint64 val);
		PrintStream& operator<< (float val);
		PrintStream& operator<< (void* val);
		PrintStream& operator<< (const char* str);
		PrintStream& operator<< (const StringBuffer& msg);
		PrintStream& operator<< (const String& str);
		PrintStream& operator<< (const StreamFlags flags);

		PrintStream* operator->() {
			return this;
		}

		// getters
		inline bool doHex() {
			return streamFlags & SF_hex;
		}

		inline bool doUpperCase() {
			return streamFlags & SF_uppercase;
		}
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*PRINTSTREAM_H_*/
