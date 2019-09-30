/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/platform.h"
#include "system/lang/String.h"

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
		constexpr const static int maxSize = 25;

		void* symbols[maxSize];

		int count;

		static String binaryName;

	public:
		StackTrace();
		StackTrace(const StackTrace& c);

		StackTrace& operator=(const StackTrace& c);

		~StackTrace();

		void print() const ;

		static void printStackTrace();

		bool equals(const StackTrace& trace) const;
		bool containsAddress(void* address) const;

		int getCount() const {
			return count;
		}

		static void setBinaryName(const String& name) {
			binaryName = name;
		}

		static const String& getBinaryName() {
			return binaryName;
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

