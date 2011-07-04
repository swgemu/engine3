/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DOUBLE_H_
#define DOUBLE_H_

#include "../../io/ObjectOutputStream.h"
#include "../../io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Double : public BaseTypeVariable<double> {
	public:
		Double() : BaseTypeVariable<double>(0) {

		}

		Double(double val) : BaseTypeVariable<double>(val) {

		}

		void parseFromString(String* str) {
			*this = valueOf(*str);
		}

		void toString(String* str) {
			*str = String::valueOf(*this);
		}

		void toBinaryStream(ObjectOutputStream* stream) {
			stream->writeFloat(get());
		}

		void parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readFloat();
		}

		static float valueOf(const String& str) {
			return atof(str.toCharArray());
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;


#endif /* DOUBLE_H_ */
