#ifndef CHARACTER_H_
#define CHARACTER_H_

#include "../platform.h"

namespace sys {
  namespace lang {

	class Character {
	public:
		static inline bool isDigit(char ch) {
			return isdigit(ch);
		}

		static inline bool isLetter(char ch) {
			return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
		}

		static inline bool isLetterOrDigit(char ch) {
			return isalpha(ch);
		}

		static inline bool isLowerCase(char ch) {
			return islower(ch);
		}

		static inline bool isUpperCase(char ch) {
			return isupper(ch);
		}

		static inline char toLowerCase(char ch) {
			if (isUpperCase(ch))
				return 'a' + (ch - 'A');
			else
				return ch;
		}

		static inline char toUpperCase(char ch) {
			if (isLowerCase(ch))
				return 'A' + (ch - 'a');
			else
				return ch;
		}

		static uint32 hashCode(char value) {
			return (uint32) value;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*CHARACTER_H_*/
