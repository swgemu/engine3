/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STRING_H_
#define STRING_H_

#include "../platform.h"

namespace sys {
  namespace lang {
		
	class String {
	public:
		static void toLower(string& str);
	
		static void toUpper(string& str);
	
		static uint32 hashCode(const string& str) {
			return hashCode(str.c_str(), str.size());
		}
		
		static uint32 hashCode(const char* str, int len);
		
		static uint64 toUnsignedLong(const char* str);
		static uint32 toHexInt(const char* str);
		
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*STRING_H_*/
