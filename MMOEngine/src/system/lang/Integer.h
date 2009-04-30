#ifndef INTEGER_H_
#define INTEGER_H_

#include "String.h"

#include "NumberFormatException.h"
#include "BaseTypeVariable.h"

#include "../io/ObjectOutputStream.h"
#include "../io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Integer : public BaseTypeVariable<int> {
	public:
		inline Integer() : BaseTypeVariable<int>(0) {

		}

		inline Integer(int val) : BaseTypeVariable<int>(val) {

		}

		void toString(String* str) {
			*str = String::valueOf(*this);
		}

		void toBinaryStream(ObjectOutputStream* stream) {
			stream->writeSignedInt(get());
		}

		void parseFromString(String* str) {
			*this = valueOf(*str);
		}

		void parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readSignedInt();
		}

		static int valueOf(char ch) {
			int digit = ch - '0';
			if (digit < 0 || digit > 9)
				throw NumberFormatException();

			return digit;
		}

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

	class UnsignedInteger: public BaseTypeVariable<unsigned int> {
	public:
		inline UnsignedInteger() : BaseTypeVariable<unsigned int>(0) {

		}

		inline UnsignedInteger(unsigned int val) : BaseTypeVariable<unsigned int>(val) {

		}

		void toString(String* str) {
			*str = String::valueOf(*this);
		}

		void toBinaryStream(ObjectOutputStream* stream) {
			stream->writeInt(get());
		}

		void parseFromString(String* str) {
			*this = (unsigned int) UnsignedInteger::valueOf(*str);
		}

		void parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readInt();
		}

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

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*INTEGER_H_*/
