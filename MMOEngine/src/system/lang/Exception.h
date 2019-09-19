/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include "system/platform.h"

#include "String.h"

#include "StackTrace.h"

namespace sys {
  namespace lang {

	class Exception {
	protected:
		String message;

		StackTrace trace;

	public:
		Exception();
		Exception(const String& msg);

		virtual ~Exception();

		void printStackTrace() const;
		void printMessage() const;

		// setters and getters
		inline void setMessage(const String& msg) {
			message = msg;
		}

	 	inline const String& getMessage() const {
			return message;
		}

		inline const StackTrace& getStackTrace() const {
			return trace;
		}

		inline const char* what() const {
			return message.toCharArray();
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*EXCEPTION_H_*/
