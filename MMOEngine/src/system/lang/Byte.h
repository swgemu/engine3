/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "Character.h"
#include "Integer.h"

#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Byte : public UnsignedCharacter {
	public:
		inline Byte() : UnsignedCharacter(0) {
		}

		inline Byte(unsigned char val) : UnsignedCharacter(val) {

		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			stream->writeByte(get());

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readByte();

			return true;
		}

		bool toString(String& str) const {
			str = String::valueOf((unsigned int)get());

			return true;
		}

		bool parseFromString(const String& str, int version = 0) {
			*this = (unsigned char) UnsignedInteger::valueOf(str);

			return true;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

