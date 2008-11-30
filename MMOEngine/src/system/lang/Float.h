/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef FLOAT_H_
#define FLOAT_H_

#include "String.h"

#include "NumberFormatException.h"

namespace sys {
  namespace lang {

	class Float {
	public:
		static float valueOf(const String& str) {
			return atof(str.toCharArray());
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*FLOAT_H_*/
