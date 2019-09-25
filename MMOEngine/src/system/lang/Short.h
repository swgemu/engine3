/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SHORT_H_
#define SHORT_H_

#include "BaseTypeVariable.h"

#include "String.h"
#include "Integer.h"

#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Short : public BaseTypeVariable<int16> {
	public:
		inline Short() : BaseTypeVariable<int16>(0) {

		}

		inline Short(int16 val) : BaseTypeVariable<int16>(val) {

		}

		inline Short(const Short& val) : BaseTypeVariable<int16>(val) {

		}

		bool parseFromString(const String& str, int version = 0) {
			*this = (int16) Integer::valueOf(str);

			return true;
		}

		bool toString(String& str) const {
			str = String::valueOf((int)*this);

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			stream->writeSignedShort(get());

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readSignedShort();

			return true;
		}

		static uint32 hashCode(int16 value) {
			return (uint32) value;
		}

	};

	class UnsignedShort : public BaseTypeVariable<uint16> {
	public:
		inline UnsignedShort() : BaseTypeVariable<uint16>(0) {

		}

		inline UnsignedShort(uint16 val) : BaseTypeVariable<uint16>(val) {

		}

		bool parseFromString(const String& str, int version = 0) {
			*this = (int16) UnsignedInteger::valueOf(str);

			return true;
		}

		bool toString(String& str) const {
			str = String::valueOf((uint32)*this);

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			stream->writeShort(get());

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readShort();

			return true;
		}

		static uint32 hashCode(uint16 value) {
			return (uint32) value;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* SHORT_H_ */
