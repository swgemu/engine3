/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "BaseTypeVariable.h"

#include "String.h"

#include "NumberFormatException.h"

#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

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

		bool toString(String& str) const {
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

		static bool areAlmostEqualRelative(float A, float B, float maxRelDiff = FLT_EPSILON) {
			float diff = fabs(A - B);

			A = fabs(A);
			B = fabs(B);

			float largest = (B > A) ? B : A;

			if (diff <= largest * maxRelDiff)
				return true;

			return false;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

