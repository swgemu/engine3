/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SERVICEEXCEPTION_H_
#define SERVICEEXCEPTION_H_

#include "system/lang/Exception.h"

namespace engine {
  namespace service {

	class ServiceException : public sys::lang::Exception {
	public:
		ServiceException() : Exception() {
		}

		ServiceException(const String& msg) : Exception(msg) {
		}
	};

  } // namespace service
} // namespace engine

using namespace engine::service;

#endif /*SERVICEEXCEPTION_H_*/
