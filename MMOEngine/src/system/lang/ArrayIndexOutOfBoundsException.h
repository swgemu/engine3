/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ARRAYOUTOFBOUNDSEXCEPTION_H_
#define ARRAYOUTOFBOUNDSEXCEPTION_H_

#include "Exception.h"

namespace sys {
  namespace lang {

	class ArrayIndexOutOfBoundsException : public Exception {
	public:
		ArrayIndexOutOfBoundsException();

		ArrayIndexOutOfBoundsException(int index);
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*ARRAYOUTOFBOUNDSEXCEPTION_H_*/
