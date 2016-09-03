/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BOOL_H_
#define BOOL_H_

#include "BaseTypeVariable.h"

#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Bool : public BaseTypeVariable<bool> {
	public:
		inline Bool() : BaseTypeVariable<bool>(false) {

		}

		inline Bool(bool val) : BaseTypeVariable<bool>(val) {

		}

		inline Bool(const Bool& val) : BaseTypeVariable<bool>(val) {

		}

		bool toString(String& str) {
			if (get() == true)
				str = String("true");
			else
				str = String("false");

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			stream->writeBoolean(get());

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readBoolean();

			return true;
		}

		bool parseFromString(const String& str, int version = 0) {
			*this = valueOf(str);

			return true;
		}

		static bool valueOf(const String& str) {
			if (str == "true") {
				return true;
			} else {
				return false;
			}
		}

		static uint32 hashCode(bool value) {
			return (uint32) value;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;


#endif /* BOOL_H_ */
