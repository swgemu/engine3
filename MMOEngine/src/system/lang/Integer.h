#ifndef INTEGER_H_
#define INTEGER_H_

#include "String.h"
#include "BaseTypeVariable.h"

namespace sys {
  namespace lang {

	class Integer : public BaseTypeVariable<int> {
	public:
		inline Integer() : BaseTypeVariable<int>(0) {

		}

		inline Integer(int val) : BaseTypeVariable<int>(val) {

		}

		inline Integer(const Integer& val) : BaseTypeVariable<int>(val) {

		}

		bool toString(String& str);

		bool toBinaryStream(ObjectOutputStream* stream);

		bool parseFromString(const String& str, int version = 0);

		bool parseFromBinaryStream(ObjectInputStream* stream);

		static void toBinaryStream(int val, ObjectOutputStream* stream);
		static void parseFromBinaryStream(int& val, ObjectInputStream* stream);

		static int valueOf(char ch);

		static int valueOf(const char* str) {
			return atoi(str);
		}

		static int valueOf(const String& str) {
			return valueOf(str.toCharArray());
		}

		static uint32 hexvalueOf(const String& str);

		static uint32 hashCode(int value) {
			return (uint32) value;
		}

	};

	class UnsignedInteger: public BaseTypeVariable<uint32> {
	public:
		inline UnsignedInteger() : BaseTypeVariable<uint32>(0) {

		}

		inline UnsignedInteger(uint32 val) : BaseTypeVariable<uint32>(val) {

		}

		inline UnsignedInteger(const UnsignedInteger& val) : BaseTypeVariable<uint32>(val) {

		}

		bool toString(String& str);

		bool toBinaryStream(ObjectOutputStream* stream);

		bool parseFromString(const String& str, int version = 0);

		bool parseFromBinaryStream(ObjectInputStream* stream);

		static unsigned int valueOf(const String& str) {
			unsigned int val = 0;
			unsigned int mul = 1;

			for (int i = str.length() - 1; i >= 0; --i) {
				int digit = Integer::valueOf(str.charAt(i));

				val += digit * mul;

				mul *= 10;
			}

			return val;
		}

		static uint32 hashCode(uint32 value) {
			return value;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*INTEGER_H_*/
