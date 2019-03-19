/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "system/lang/System.h"

#include "system/thread/Thread.h"

#include "engine/log/Logger.h"
#include "engine/core/Core.h"

String StackTrace::binaryName = "core3";

StackTrace::StackTrace() {
	#ifdef PLATFORM_UNIX
		count = backtrace(symbols, 25);
	#endif
}

StackTrace::StackTrace(const StackTrace& c) {
	count = c.count;
	memcpy(symbols, c.symbols, sizeof(void*) * c.count);
}

StackTrace& StackTrace::operator=(const StackTrace& c) {
	if (this == &c)
		return *this;

	count = c.count;
	memcpy(symbols, c.symbols, sizeof(void*) * c.count);

	return *this;
}

StackTrace::~StackTrace() {

}

void StackTrace::print() const {
	static int initializeProperties = Core::initializeProperties("StackTrace");
	static Logger logger("StackTrace");

	#ifdef PLATFORM_UNIX
		static const bool enableAddr2Line = Core::getIntProperty("StackTrace.enableArr2Line", 1);
		char** tracedSymbols = backtrace_symbols(symbols, count);

		if (tracedSymbols == nullptr) {
			System::out << "error while trying to print stack trace: tracedSymbols == nullptr" << endl;
			return;
		}

		#ifdef LINE_TRACING
			StringBuffer command;

			#ifdef PLATFORM_MAC
			command << "atos -p " << Thread::getProcessID();
			#else
			command << "/usr/bin/addr2line -e " << binaryName;
			#endif
		#endif

		StringBuffer lines;
		for (int i = 0; i < count; ++i) {
			if (enableAddr2Line) {
			#ifdef LINE_TRACING
				command << " " << hex << symbols[i];
			#endif
			}

			lines << tracedSymbols[i] << endl;
		}

		free(tracedSymbols);

	#ifdef LINE_TRACING
		if (enableAddr2Line) {
			auto res = system(command.toString().toCharArray());
		}
	#endif

		logger.warning(lines.toString());
	#elif defined PLATFORM_CYGWIN
		cygwin_stackdump();
	#endif
}

void StackTrace::getStackTrace(String& trace) const {
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

bool StackTrace::equals(const StackTrace& trace) const {
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
