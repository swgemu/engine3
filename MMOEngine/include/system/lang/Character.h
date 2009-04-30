#ifndef CHARACTER_H_
#define CHARACTER_H_

#include "../platform.h"

#include "BaseTypeVariable.h"

#include "String.h"

#include "../io/ObjectOutputStream.h"
#include "../io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Character : public BaseTypeVariable<char> {
	public:
		inline Character() : BaseTypeVariable<char>(0) {

		}

		inline Character(char val) : BaseTypeVariable<char>(val) {

		}

		void toString(String* str) {
			*str = String::valueOf(*this);
		}

		void parseFromString(String* str) {
			*this = valueOf(*str);
		}

		void toBinaryStream(ObjectOutputStream* stream) {
			stream->writeSignedByte(get());
		}

		void parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readSignedByte();
		}

		static inline char valueOf(String& str) {
			return str.charAt(0);
		}

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

	class UnsignedCharacter : public BaseTypeVariable<unsigned char> {
	public:
		inline UnsignedCharacter() : BaseTypeVariable<unsigned char>(0) {

		}

		inline UnsignedCharacter(unsigned char val) : BaseTypeVariable<unsigned char>(val) {

		}

		void toString(String* str) {
			*str = String::valueOf(*this);
		}

		void parseFromString(String* str) {
			*this = Character::valueOf(*str);
		}

		void toBinaryStream(ObjectOutputStream* stream) {
			stream->writeByte(get());
		}

		void parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readByte();
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*CHARACTER_H_*/
