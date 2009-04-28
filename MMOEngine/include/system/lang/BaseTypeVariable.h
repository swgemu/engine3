/*
 * BaseTypeVariable.h
 *
 *  Created on: Apr 25, 2009
 *      Author: theanswer
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

			virtual ~BaseTypeVariable() {
			}

			virtual void toString(String& str) = 0;

			inline virtual int compareTo(const O& val) const {
				if (object < val)
					return 1;
				else if (object > val)
					return -1;
				else return 0;
			}

			inline virtual bool operator== (const O& val) const {
				return object == val;
			}

			inline virtual bool operator< (const O& val) const {
				return object < val;
			}

			inline virtual bool operator> (const O& val) const {
				return object > val;
			}

			inline virtual bool operator!= (const O& val) const {
				return object != val;
			}

			inline void operator=(O obj) {
				object = obj;
			}

			inline virtual O get() {
				return object;
			}

			operator O() {
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
