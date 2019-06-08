/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef ILLEGALARGUMENTEXCEPTION_H_
#define ILLEGALARGUMENTEXCEPTION_H_

#include "Exception.h"

namespace sys {
  namespace lang {

	class IllegalArgumentException : public Exception {
	public:
		IllegalArgumentException() : Exception("IllegalArgumentException") {
		}

		IllegalArgumentException(String s) : Exception("IllegalArgumentException") {
			message = message + " - " + s;
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*ILLEGALARGUMENTEXCEPTION_H_*/
