/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef ILLEGALSTATEEXCEPTION_H_
#define ILLEGALSTATEEXCEPTION_H_

#include "Exception.h"

namespace sys {
  namespace lang {

	class IllegalStateException : public Exception {
	public:
		IllegalStateException() : Exception("IllegalStateException") {
		}

		IllegalStateException(String s) : Exception("IllegalStateException") {
			message = message + " - " + s;
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*ILLEGALSTATEEXCEPTION_H_*/

