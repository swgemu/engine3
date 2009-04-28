/*
 * Variable.h
 *
 *  Created on: Apr 25, 2009
 *      Author: theanswer
 */

#ifndef VARIABLE_H_
#define VARIABLE_H_

#include "../platform.h"

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
		virtual void toString(String* str) {

		}

		virtual void toBinaryData(sys::io::ObjectOutputStream* stream) {

		}

	};

	} // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* VARIABLE_H_ */
