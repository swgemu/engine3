/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STACKTRACE_H_
#define STACKTRACE_H_

#include "system/platform.h"

#ifdef PLATFORM_UNIX
	#include <execinfo.h>
#elif defined PLATFORM_CYGWIN
	extern "C" void cygwin_stackdump(void);
#endif

#ifdef TRACE_REFERENCING
	#define LINE_TRACING
#endif

/*#ifdef PLATFORM_MAC
	#undef LINE_TRACING
#endif*/

namespace sys {
  namespace lang {

	class StackTrace {
		void* symbols[25];

		int count;

		static String binaryName;

	public:
		StackTrace();
		StackTrace(const StackTrace& c);

		StackTrace& operator=(const StackTrace& c);

		~StackTrace();

		void print() const ;

		void getStackTrace(String& trace) const;

		static void printStackTrace();

		bool equals(const StackTrace& trace) const;

		static void setBinaryName(const String& name) {
			binaryName = name;
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*STACKTRACE_H_*/
