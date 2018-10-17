/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef NUMBERFORMATEXCEPTION_H_
#define NUMBERFORMATEXCEPTION_H_

#include "Exception.h"

#include "StringBuffer.h"

namespace sys {
  namespace lang {

	class NumberFormatException : public Exception {
	public:
		NumberFormatException() : Exception("NumberFormatException\n") {
		}

		NumberFormatException(int index) : Exception() {
			StringBuffer str;
			str << "NumberFormatException at " << index << "\n";
			message = str.toString();
		}

		NumberFormatException(int index, const String& num) : Exception() {
			StringBuffer str;
			str << "NumberFormatException at " << index << " while converting String:[ " << num << "]\n";
			message = str.toString();
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*NUMBERFORMATEXCEPTION_H_*/
