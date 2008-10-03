/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LONG_H_
#define LONG_H_

namespace sys {
  namespace lang {

	class Long {
	public:
		static int hashCode(uint64 value) {
			return (int)(value ^ (value >> 32));	
		}
		
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*LONG_H_*/
