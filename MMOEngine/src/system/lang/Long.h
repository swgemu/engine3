/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LONG_H_
#define LONG_H_

#include "String.h"

#include "NumberFormatException.h"

namespace sys {
  namespace lang {

	class Long {
	public:
		static int hashCode(uint64 value) {
			return (int)(value ^ (value >> 32));
		}

		static int64 valueOf(const String& str) {
			int64 val = 0;
			int64 mul = 1;

			for (int i = str.length() - 1; i >= 0; --i) {
				char ch = str.charAt(i);

				if (i == 0 && ch == '-') {
					val *= -1;
					break;
				}

				int digit = ch - '0';

				if (digit < 0 || digit > 9)
					throw NumberFormatException(i);

				val += digit * mul;

				mul *= 10;
			}

			return val;
		}

		static uint64 unsignedvalueOf(const String& str) {
			uint64 val = 0;
			uint64 mul = 1;

			for (int i = str.length() - 1; i >= 0; --i) {
				int digit = str.charAt(i) - '0';

				if (digit < 0 || digit > 9)
					throw NumberFormatException(i);

				val += digit * mul;

				mul *= 10;
			}

			return val;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*LONG_H_*/
