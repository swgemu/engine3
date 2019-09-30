/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#pragma once

#include "system/platform.h"

#include "BaseTypeVariable.h"

#include "String.h"

#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	class Character : public BaseTypeVariable<char> {
	public:
		inline Character() : BaseTypeVariable<char>(0) {

		}

		inline Character(char val) : BaseTypeVariable<char>(val) {

		}

		inline Character(const Character& val) : BaseTypeVariable<char>(val) {

		}

		bool toString(String& str) const {
			str = String::valueOf(*this);

			return true;
		}

		bool parseFromString(const String& str, int version = 0) {
			*this = valueOf(str);

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			stream->writeSignedByte(get());

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readSignedByte();

			return true;
		}

		static inline char valueOf(const String& str) {
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

		inline UnsignedCharacter(const UnsignedCharacter& val) : BaseTypeVariable<unsigned char>(val) {

		}

		bool toString(String& str) const {
			str = String::valueOf(*this);

			return true;
		}

		bool parseFromString(const String& str, int version = 0) {
			*this = Character::valueOf(str);

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			stream->writeByte(get());

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			*this = stream->readByte();

			return true;
		}

		static uint32 hashCode(unsigned char value) {
			return (uint32) value;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

