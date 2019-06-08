/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef OUTOFMEMORYERROR_H_
#define OUTOFMEMORYERROR_H_

#include "Exception.h"

namespace sys {
  namespace lang {

	class OutOfMemoryError : public Exception {
	public:
		OutOfMemoryError() : Exception("OutOfMemoryError\n") {
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*OUTOFMEMORYERROR_H_*/
