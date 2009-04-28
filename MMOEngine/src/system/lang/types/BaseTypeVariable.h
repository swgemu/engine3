/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASETYPEVARIABLE_H_
#define BASETYPEVARIABLE_H_

#include "Variable.h"
#include "String.h"

namespace sys {
	namespace lang {

		template <class O> class BaseTypeVariable : public Variable {
			O object;

		public:
			BaseTypeVariable() : Variable() {

			}

			BaseTypeVariable(O defaultValue) : Variable() {
				initialize(defaultValue);
			}

			inline virtual int compareTo(const O& val) const {
				if (object < val)
					return 1;
				else if (object > val)
					return -1;
				else return 0;
			}

			inline O operator|=(O obj) {
				return object |= obj;
			}

			inline O operator^=(O obj) {
				return object ^= obj;
			}

			inline O operator&=(O obj) {
				return object &= obj;
			}

			inline O operator>>=(O obj) {
				return object >>= obj;
			}

			inline O operator<<=(O obj) {
				return object <<= obj;
			}

			inline O operator%=(O obj) {
				return object %= obj;
			}

			inline O operator/=(O obj) {
				return object /= obj;
			}

			inline O operator*=(O obj) {
				return object *= obj;
			}

			inline O operator+=(O obj) {
				return object += obj;
			}

			inline O operator-=(O obj) {
				return object -= obj;
			}

			inline void operator=(O obj) {
				object = obj;
			}

			inline virtual O get() {
				return object;
			}

			inline operator O() {
				return object;
			}

		protected:
			inline void initialize(const O& value) {
				object = value;
			}
		};

	}
}

using namespace sys::lang;


#endif /* BASETYPEVARIABLE_H_ */
