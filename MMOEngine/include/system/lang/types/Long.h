/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LONG_H_
#define LONG_H_

#include "Integer.h"

#include "String.h"
#include "../StringBuffer.h"

#include "../NumberFormatException.h"

#include "BaseTypeVariable.h"

#include "../../io/ObjectOutputStream.h"
#include "../../io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	  class Long : public BaseTypeVariable<int64> {
	  public:
		  Long() : BaseTypeVariable<int64>(0) {

		  }

		  Long(int64 val) : BaseTypeVariable<int64>(val) {

		  }

		  void toString(String* str) {
			  *str = String::valueOf(*this);
		  }

		  void parseFromString(String* str) {
			  *this = valueOf(*str);
		  }

		  void toBinaryStream(ObjectOutputStream* stream) {
			  stream->writeSignedLong(get());
		  }

		  void parseFromBinaryStream(ObjectInputStream* stream) {
			  *this = stream->readSignedLong();
		  }

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

				  val += Integer::valueOf(ch) * mul;

				  mul *= 10;
			  }

			  return val;
		  }

		  static uint64 unsignedvalueOf(const String& str) {
			  uint64 val = 0;
			  uint64 mul = 1;

			  for (int i = str.length() - 1; i >= 0; --i) {
				  int digit = Integer::valueOf(str.charAt(i));

				  val += digit * mul;

				  mul *= 10;
			  }

			  return val;
		  }

		  static String toString(int64 val) {
			  String str;

			  toString(str, val);

			  return str;
		  }

		  static void toString(String& str, int64 val) {
			  char buf[32];

			  snprintf(buf, 32, "%lld", val);

			  str = buf;
		  }

		  static String toString(uint64 val) {
			  String str;

			  toString(str, val);

			  return str;
		  }

		  static void toString(String& str, uint64 val) {
			  char buf[32];

			  snprintf(buf, 32, "%llu", val);

			  str = buf;
		  }
	  };

	  class UnsignedLong : public BaseTypeVariable<uint64> {
	  public:
		  UnsignedLong() : BaseTypeVariable<uint64>(0) {

		  }

		  UnsignedLong(uint64 val) : BaseTypeVariable<uint64>(val) {

		  }

		  void toString(String* str) {
			  *str = String::valueOf(*this);
		  }

		  void parseFromString(String* str) {
			  *this = UnsignedLong::valueOf(*str);
		  }

		  void toBinaryStream(ObjectOutputStream* stream) {
			  stream->writeLong(BaseTypeVariable<uint64>::get());
		  }

		  void parseFromBinaryStream(ObjectInputStream* stream) {
			  *this = stream->readLong();
		  }

		  static uint64 valueOf(const String& str) {
			  uint64 val = 0;
			  uint64 mul = 1;

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

#endif /*LONG_H_*/
