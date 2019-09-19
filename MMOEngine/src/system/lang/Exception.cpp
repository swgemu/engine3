/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "System.h"

#include "SignalException.h"

//SignalTranslator<AbortedException> g_objAbortedExceptionTranslator;
//SignalTranslator<FloatingPointException> _objFloatingPOintExceptionTranslator;

Exception::Exception() {
}

Exception::Exception(const String& msg) : message(msg) {
}

Exception::~Exception() {
}

void Exception::printStackTrace() const {
	System::out << message << endl;
	trace.print();

	System::out << flush;
}

void Exception::printMessage() const {
	System::out << message << endl;
}
