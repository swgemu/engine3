/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SIGNALEXCEPTION_H_
#define SIGNALEXCEPTION_H_

#ifndef __CYGWIN__

#include "Exception.h"

#include "System.h"

#include <signal.h>

template <class SignalExceptionClass> class SignalTranslator {
private:
	class SingleTonTranslator {
	public:
		SingleTonTranslator() {
			signal(SignalExceptionClass::GetSignalNumber(), SignalHandler);
		}

		static void SignalHandler(int) {
			throw SignalExceptionClass();
		}
	};

public:
	SignalTranslator() {
		static SingleTonTranslator s_objTranslator;
	}
};

class SegmentationFault : public Exception {
public:
	SegmentationFault() : Exception() {
		System::out << "Segmentation Fault at\n";
		printStackTrace();
	}

	static int GetSignalNumber() {
		return SIGSEGV;
	}
};

SignalTranslator<SegmentationFault> g_objSegmentationFaultTranslator;

class FloatingPointException : public Exception {
public:
	FloatingPointException() :Exception() {
		printStackTrace();
	}

	static int GetSignalNumber() {
		return SIGFPE;
	}
};

SignalTranslator<FloatingPointException> _objFloatingPOintExceptionTranslator;

#endif

#endif /*SIGNALEXCEPTION_H_*/
