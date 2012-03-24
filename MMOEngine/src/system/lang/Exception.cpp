#include "System.h"

#include "Exception.h"

#include "SignalException.h"

SignalTranslator<AbortedException> g_objAbortedExceptionTranslator;
SignalTranslator<FloatingPointException> _objFloatingPOintExceptionTranslator;

Exception::Exception() {
}

Exception::Exception(const String& msg) {
	message = msg;
}

Exception::~Exception() {
}

void Exception::printStackTrace() const {
	System::out << message << "\n";
	trace.print();
}
