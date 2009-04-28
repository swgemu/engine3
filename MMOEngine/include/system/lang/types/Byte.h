/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BYTE_H_
#define BYTE_H_

#include "Character.h"
#include "Integer.h"

namespace sys {
  namespace lang {

	class Byte : public UnsignedCharacter {
	public:
		Byte() : UnsignedCharacter(0) {

		}

		Byte(unsigned char val) : UnsignedCharacter(val) {

		}

		void toString(String* str) {
			*str = String::valueOf((unsigned int)get());
		}

		void parseFromString(String* str) {
			*this = (unsigned char) UnsignedInteger::valueOf(*str);
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;


#endif /* BYTE_H_ */
