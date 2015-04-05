/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef FLOAT_H_
#define FLOAT_H_

#include "BaseTypeVariable.h"

#include "String.h"

#include "NumberFormatException.h"

#include "../io/ObjectOutputStream.h"
#include "../io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Float : public BaseTypeVariable<float> {
	public:
		inline Float() : BaseTypeVariable<float>(0.f) {

		}

		inline Float(float val) : BaseTypeVariable<float>(val) {

		}

		inline Float(const Float& val) : BaseTypeVariable<float>(val) {

		}

		bool parseFromString(const String& str, int version = 0) {
			*this = valueOf(str);

			return true;
		}

		bool toString(String& str) {
			str = String::valueOf(*this);

			return true;
		}

		bool toBinaryStream(sys::io::ObjectOutputStream* stream) {
			stream->writeFloat(BaseTypeVariable<float>::get());

			return true;
		}

		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream) {
			*this = stream->readFloat();

			return true;
		}

		static float valueOf(const String& str) {
			return atof(str.toCharArray());
		}

		static uint32 hashCode(float value) {
			//TODO
			return (uint32) value;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*FLOAT_H_*/
