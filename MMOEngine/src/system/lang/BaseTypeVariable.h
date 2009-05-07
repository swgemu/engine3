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
			inline BaseTypeVariable() : Variable() {

			}

			inline BaseTypeVariable(O defaultValue) : Variable() {
				initialize(defaultValue);
			}

			inline BaseTypeVariable(const BaseTypeVariable& val) : Variable() {
				initialize(val.object);
			}

			virtual int compareTo(const BaseTypeVariable& val) {
				if (object < val.object)
					return 1;
				else if (object > val.object)
					return -1;
				else return 0;

				return 1;
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

			inline O operator++(int) {
				return object++;
			}

			inline O operator--(int) {
				return object--;
			}

			inline O operator++() {
				return ++object;
			}

			inline O operator--() {
				return --object;
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
