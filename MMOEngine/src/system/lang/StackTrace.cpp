/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../lang/System.h"

#include "../io/StringTokenizer.h"

#include "../thread/Thread.h"

#include "StackTrace.h"

String StackTrace::binaryName = "core3";

StackTrace::StackTrace() {
	#ifdef PLATFORM_UNIX
		count = backtrace(symbols, 25);
	#endif
}

StackTrace::~StackTrace() {

}

void StackTrace::print() {
	#ifdef PLATFORM_UNIX
		#ifdef LINE_TRACING
			StringBuffer command;

			#ifdef PLATFORM_MAC
			command << "atos -p " << Thread::getProcessID();
			#else
			command << "/usr/bin/addr2line -e " << binaryName;
			#endif
		#else
		char** tracedSymbols = backtrace_symbols(symbols, count);

		if (tracedSymbols == NULL) {
			System::out << "error while trying to print stack trace: tracedSymbols == NULL" << endl;
			return;
		}

		#endif
		for (int i = 0; i < count; ++i) {
			#ifdef LINE_TRACING
				command << " " << hex << symbols[i];
			#else
				System::out << tracedSymbols[i] << endl;
			#endif
		}
		#ifdef LINE_TRACING
			system(command.toString().toCharArray());
		#else
			free(tracedSymbols);
		#endif
	#elif defined PLATFORM_CYGWIN
		cygwin_stackdump();
	#endif
}

void StackTrace::getStackTrace(String& trace) {
	StringBuffer out;

	#ifdef PLATFORM_UNIX
		for (int i = 0; i < count; i++) {
		}
	#endif

	trace = out.toString();
}

void StackTrace::printStackTrace() {
	StackTrace trace;
	trace.print();
}

bool StackTrace::equals(const StackTrace& trace) {
	if (count != trace.count)
		return false;

	for (int i = 0; i < count; ++i) {
		void* symbol1 = symbols[i];
		void* symbol2 = trace.symbols[i];

		if (symbol1 != symbol2)
			return false;
	}

	return true;
}
