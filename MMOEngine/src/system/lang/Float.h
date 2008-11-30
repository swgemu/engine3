/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LONG_H_
#define LONG_H_

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

#endif /*LONG_H_*/
