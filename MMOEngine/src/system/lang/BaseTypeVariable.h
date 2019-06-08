/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

			inline BaseTypeVariable(const O& defaultValue) : Variable() {
				initialize(defaultValue);
			}

			inline BaseTypeVariable(const BaseTypeVariable& val) : Variable() {
				initialize(val.object);
			}

			virtual int compareTo(const BaseTypeVariable& val) const {
				if (object < val.object)
					return 1;
				else if (object > val.object)
					return -1;
				else
					return 0;
			}

			virtual int compareTo(const BaseTypeVariable* val) const {
				if (object < val->object)
					return 1;
				else if (object > val->object)
					return -1;
				else
					return 0;
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

			inline O get() const {
				return object;
			}

			inline operator O() const {
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
