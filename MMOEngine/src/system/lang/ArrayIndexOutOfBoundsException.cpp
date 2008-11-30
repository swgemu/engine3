#include "StringBuffer.h"

#include "ArrayIndexOutOfBoundsException.h"

ArrayIndexOutOfBoundsException::ArrayIndexOutOfBoundsException() : Exception("ArrayIndexOutOfBoundsException\n") {
}

ArrayIndexOutOfBoundsException::ArrayIndexOutOfBoundsException(int index) :	Exception() {
	StringBuffer str;
	str << "ArrayIndexOutOfBoundsException at " << index << "\n";
	message = str.toString();
}
