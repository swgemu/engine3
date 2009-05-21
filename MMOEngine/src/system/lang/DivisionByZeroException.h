/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DIVISIONBYZEROEXCEPTION_H_
#define DIVISIONBYZEROEXCEPTION_H_

#include "Exception.h"

namespace sys {
  namespace lang {

	class DivisionByZeroException : public Exception {
	public:
		DivisionByZeroException() : Exception("DivisionByZeroException") {
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*DIVISIONBYZEROEXCEPTION_H_*/
