/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STACKTRACE_H_
#define STACKTRACE_H_

#include "../platform.h"

#ifdef PLATFORM_UNIX
	#include <execinfo.h>
#elif defined PLATFORM_CYGWIN
	extern "C" void cygwin_stackdump(void);
#endif

#ifdef TRACE_REFERENCING
	#define LINE_TRACING
#endif

namespace sys {
  namespace lang {

	class StackTrace {
	#ifdef LINE_TRACING
		void** symbols;
	#else
		char** symbols;
	#endif

		int count;

	public:
		StackTrace();

		~StackTrace();

		void print();

		void getStackTrace(String& trace);

		static void printStackTrace();

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*STACKTRACE_H_*/
