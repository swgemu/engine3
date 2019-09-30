/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#pragma once

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

		bool toString(String& str) const;

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
			uint32 a = value;

			a = (a+0x7ed55d16) + (a<<12);
			a = (a^0xc761c23c) ^ (a>>19);
			a = (a+0x165667b1) + (a<<5);
			a = (a+0xd3a2646c) ^ (a<<9);
			a = (a+0xfd7046c5) + (a<<3);
			a = (a^0xb55a4f09) ^ (a>>16);

			return a;
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

		bool toString(String& str) const;

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
			uint32 a = value;

			a = (a+0x7ed55d16) + (a<<12);
			a = (a^0xc761c23c) ^ (a>>19);
			a = (a+0x165667b1) + (a<<5);
			a = (a+0xd3a2646c) ^ (a<<9);
			a = (a+0xfd7046c5) + (a<<3);
			a = (a^0xb55a4f09) ^ (a>>16);

			return a;
		}

	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

