/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../io/StringTokenizer.h"

#include "StackTrace.h"

StackTrace::StackTrace() {
	#ifdef PLATFORM_UNIX
		void* array[25];
		count = backtrace(array, 25);
		
		#ifdef LINE_TRACING
			char** syms = backtrace_symbols(array, count);

			symbols = new uint32[count];
		
			for (int i = 0; i < count; i++) {
				string line(syms[i]);

				try {
					StringTokenizer tokenizer(line.substr(2, line.size() - 1));			
					symbols[i] = tokenizer.getHexIntToken();
				} catch (...) {
					cout << "ERROR - Exception on Stack Traceing\n";
				}
			}
		
			free(syms);
		#else
			symbols = backtrace_symbols(array, count);
		#endif
	#endif
}

StackTrace::~StackTrace() {
	#ifdef PLATFORM_UNIX
		if (symbols != NULL) {
			#ifdef LINE_TRACING
				delete [] symbols;
			#else
				free(symbols);
			#endif

			symbols = NULL;
		}
	#endif
}

void StackTrace::print() {
	#ifdef PLATFORM_UNIX
		for (int i = 0; i < count; i++) {
			#ifdef LINE_TRACING
				stringstream command;
				command << "/usr/bin/addr2line -e core3 0x"<< hex << symbols[i] << dec;

				system(command.str().c_str());
			#else
				cout << symbols[i];
				
				/*string line(symbols[i]);
			
				string address = line.substr(2, line.find(" "));

				system("/usr/bin/addr2line","addr2line", "-e", "core3", address.c_str());*/
			#endif				
						
			cout << endl;
		}
	#elif defined PLATFORM_CYGWIN
		cygwin_stackdump();
	#endif	
}

void StackTrace::getStackTrace(string& trace) {
	stringstream out;
	
	#ifdef PLATFORM_UNIX
		for (int i = 0; i < count; i++) {
		}
	#endif
	
	trace = out.str();
}

void StackTrace::printStackTrace() {
	StackTrace trace;
	trace.print();
}	
