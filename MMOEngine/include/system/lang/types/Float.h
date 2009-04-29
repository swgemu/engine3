/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef FLOAT_H_
#define FLOAT_H_

#include "BaseTypeVariable.h"

#include "String.h"

#include "../NumberFormatException.h"

#include "../../io/ObjectOutputStream.h"
#include "../../io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Float : public BaseTypeVariable<float> {
	public:
		Float() : BaseTypeVariable<float>(0.f) {

		}

		Float(float val) : BaseTypeVariable<float>(val) {

		}

		void parseFromString(String* str) {
			*this = valueOf(*str);
		}

		void toString(String* str) {
			*str = String::valueOf(*this);
		}

		void toBinaryStream(sys::io::ObjectOutputStream* stream) {
			stream->writeFloat(BaseTypeVariable<float>::get());
		}

		void parseFromBinaryStream(sys::io::ObjectInputStream* stream) {
			*this = stream->readFloat();
		}

		static float valueOf(const String& str) {
			return atof(str.toCharArray());
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*FLOAT_H_*/
