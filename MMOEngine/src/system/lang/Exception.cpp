#include "System.h"

#include "Exception.h"

Exception::Exception() {
}

Exception::Exception(const String& msg) {
	message = msg;
}

Exception::~Exception() {
}

void Exception::printStackTrace() {
	System::out << message << "\n";
	trace.print();
}
