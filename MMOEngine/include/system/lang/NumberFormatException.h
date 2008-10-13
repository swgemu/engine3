/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef NUMBERFORMATEXCEPTION_H_
#define NUMBERFORMATEXCEPTION_H_

#include "Exception.h"

namespace sys {
  namespace lang {

	class NumberFormatException : public Exception {
	public:
		NumberFormatException() : Exception("NumberFormatException\n") {
		}

		NumberFormatException(int index) : Exception() {
			stringstream sstr;
			sstr << "NumberFormatException at " << index << "\n";
			message = sstr.str();
		}

		NumberFormatException(int index, const string& str) : Exception() {
			stringstream sstr;
			sstr << "NumberFormatException at " << index << " while converting string:[ " << str << "]\n";
			message = sstr.str();
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*NUMBERFORMATEXCEPTION_H_*/
