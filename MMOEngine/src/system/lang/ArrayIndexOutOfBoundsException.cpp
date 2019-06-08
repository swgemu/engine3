/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "StringBuffer.h"

ArrayIndexOutOfBoundsException::ArrayIndexOutOfBoundsException() : Exception("ArrayIndexOutOfBoundsException\n") {
}

ArrayIndexOutOfBoundsException::ArrayIndexOutOfBoundsException(int index) :	Exception() {
	StringBuffer str;
	str << "ArrayIndexOutOfBoundsException at " << index << "\n";
	message = str.toString();
}
