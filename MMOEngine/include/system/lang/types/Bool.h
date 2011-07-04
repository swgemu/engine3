/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BOOL_H_
#define BOOL_H_

#include "BaseTypeVariable.h"

#include "../../io/ObjectOutputStream.h"
#include "../../io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Bool : public BaseTypeVariable<bool> {
	public:
		Bool() : BaseTypeVariable<bool>(false) {

		}

		Bool(bool val) : BaseTypeVariable<bool>(val) {

		}

		void toString(String* str) {
			if (get() == true)
				*str = String("true");
			else
				*str = String("false");
		}

		void toBinaryStream(ObjectOutputStream* stream) {
			stream->writeBoolean(get());
		}

		void parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readBoolean();
		}

		void parseFromString(String* str) {
			if (*str == "true") {
				*this = true;
			} else {
				*this = false;
			}
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;


#endif /* BOOL_H_ */
