/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/lang/System.h"

#include "system/thread/Thread.h"

#include "engine/log/Logger.h"
#include "engine/core/Core.h"

String StackTrace::binaryName = "core3";

StackTrace::StackTrace() {
	#ifdef PLATFORM_UNIX
		count = backtrace(symbols, maxSize);
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

bool StackTrace::containsAddress(const void* address) const {
	for (int i = 0; i < count; ++i) {
		if (symbols[i] == address) {
			return true;
		}
	}

	return false;
}

void StackTrace::print() const {
	static const int initializeProperties = Core::initializeProperties("StackTrace");
	static Logger logger("StackTrace");

#ifdef PLATFORM_UNIX
	static const bool enableAddr2Line = Core::getIntProperty("StackTrace.enableAddr2Line", 1);
	static const String addr2linePath = Core::getProperty("StackTrace.addr2linePath", "/usr/bin/addr2line");
	static const String configBinaryName = Core::getProperty("StackTrace.binaryName", binaryName);

	char** tracedSymbols = backtrace_symbols(symbols, count);

	if (tracedSymbols == nullptr) {
		logger.error() << "error while trying to print stack trace: tracedSymbols == nullptr" << endl;
		return;
	}

#ifdef LINE_TRACING
	StringBuffer command;

#ifdef PLATFORM_MAC
	command << "atos -p " << Thread::getProcessID();
#else
	command << addr2linePath << " -e " << binaryName;
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
