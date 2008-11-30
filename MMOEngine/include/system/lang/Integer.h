#ifndef INTEGER_H_
#define INTEGER_H_

#include "String.h"

#include "NumberFormatException.h"

namespace sys {
  namespace lang {

	class Integer {
	public:
		static int valueOf(const char* str) {
			return atoi(str);
		}

		static int valueOf(const String& str) {
			return valueOf(str.toCharArray());
		}

		static uint32 hexvalueOf(const String& str) {
			uint64 val = 0;
			uint64 mul = 1;

			for (int i = str.length() - 1; i >= 0; --i) {
				char c = str.charAt(i);
				int digit;

				if (c >= '0' && c <= '9')
					digit = c - '0';
				else if (c >= 'A' && c <= 'F')
					digit = c - 'A' + 10;
				else if (c >= 'a' && c <= 'f')
					digit = c - 'a' + 10;
				else
					throw NumberFormatException(i);

				val += digit * mul;

				mul *= 16;
			}

			return val;
		}

		static uint32 hashCode(int value) {
			return (uint32) value;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*INTEGER_H_*/
