/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

		static void SignalHandler(int cause) {
			throw SignalExceptionClass(cause);
		}
	};

public:
	SignalTranslator() {
		static SingleTonTranslator s_objTranslator;
	}
};

template <class SignalCallbackClass> class SignalCallbackTranslator {
	private:
		class SingleTonTranslator {
			public:
				SingleTonTranslator() {
					signal(SignalCallbackClass::GetSignalNumber(), SignalCallbackClass::SignalHandler);
				}
		};

	public:
		SignalCallbackTranslator() {
			static SingleTonTranslator s_objTranslator;
		}
};

class AbortedException : public Exception {
public:
	AbortedException(int cause) : Exception() {
		System::out << "Exception aborted with cause " << cause << " at\n";
		printStackTrace();

		exit(1);
	}

	static int GetSignalNumber() {
		return SIGABRT;
	}
};

class FloatingPointException : public Exception {
public:
	FloatingPointException(int) :Exception() {
		printStackTrace();
	}

	static int GetSignalNumber() {
		return SIGFPE;
	}
};

#endif

#endif /*SIGNALEXCEPTION_H_*/
