/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SHORT_H_
#define SHORT_H_

#include "BaseTypeVariable.h"

#include "String.h"
#include "Integer.h"

#include "../io/ObjectOutputStream.h"
#include "../io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Short : public BaseTypeVariable<int16> {
	public:
		Short() : BaseTypeVariable<int16>(0) {

		}

		Short(int16 val) : BaseTypeVariable<int16>(val) {

		}

		void parseFromString(String* str) {
			*this = (int16) Integer::valueOf(*str);
		}

		void toString(String* str) {
			*str = String::valueOf((int)*this);
		}

		void toBinaryStream(ObjectOutputStream* stream) {
			stream->writeSignedShort(get());
		}

		void parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readSignedShort();
		}

	};

	class UnsignedShort : public BaseTypeVariable<uint16> {
	public:
		UnsignedShort() : BaseTypeVariable<uint16>(0) {

		}

		UnsignedShort(uint16 val) : BaseTypeVariable<uint16>(val) {

		}

		void parseFromString(String* str) {
			*this = (int16) UnsignedInteger::valueOf(*str);
		}

		void toString(String* str) {
			*str = String::valueOf((uint32)*this);
		}

		void toBinaryStream(ObjectOutputStream* stream) {
			stream->writeShort(get());
		}

		void parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readShort();
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* SHORT_H_ */
