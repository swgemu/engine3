/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef VARIABLE_H_
#define VARIABLE_H_

#include "../../platform.h"

namespace sys {
	namespace io {
		class ObjectOutputStream;
	}
}

namespace sys {
	namespace lang {

	class String;

	class Variable {
	public:
		virtual ~Variable() {

		}

		virtual void toString(String* str) = 0;

		virtual void parseFromString(String* str) = 0;

		virtual bool parseFromString(String* str, int version) {
			return false;
		}

		virtual void toBinaryData(sys::io::ObjectOutputStream* stream) {

		}

	};

	} // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* VARIABLE_H_ */
