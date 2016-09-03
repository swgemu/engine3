/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef LONG_H_
#define LONG_H_

#include "Integer.h"

#include "String.h"
#include "StringBuffer.h"

#include "NumberFormatException.h"

#include "BaseTypeVariable.h"

#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

namespace sys {
  namespace lang {

	  class Long : public BaseTypeVariable<int64> {
	  public:
		  inline Long() : BaseTypeVariable<int64>(0) {

		  }

		  inline Long(int64 val) : BaseTypeVariable<int64>(val) {

		  }

		  inline Long(const Long& val) : BaseTypeVariable<int64>(val) {

		  }

		  bool toString(String& str) {
			  str = String::valueOf(*this);

			  return true;
		  }

		  bool parseFromString(const String& str, int version = 0) {
			  *this = valueOf(str);

			  return true;
		  }

		  bool toBinaryStream(ObjectOutputStream* stream) {
			  stream->writeSignedLong(get());

			  return true;
		  }

		  bool parseFromBinaryStream(ObjectInputStream* stream) {
			  *this = stream->readSignedLong();

			  return true;
		  }

		  static unsigned int hashCode(uint64 value) {
			  return (unsigned int)(value ^ (value >> 32));
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

#ifdef PLATFORM_WIN
			  snprintf(buf, 32, "%I64d", val);
#else
			  snprintf(buf, 32, "%lld", val);
#endif

			  str = buf;
		  }

		  static String toString(uint64 val) {
			  String str;

			  toString(str, val);

			  return str;
		  }

		  static void toString(String& str, uint64 val) {
			  char buf[32];

#ifdef PLATFORM_WIN
			  snprintf(buf, 32, "%I64u", val);
#else
			  snprintf(buf, 32, "%llu", val);
#endif


			  str = buf;
		  }
	  };

	  class UnsignedLong : public BaseTypeVariable<uint64> {
	  public:
		  inline UnsignedLong() : BaseTypeVariable<uint64>(0) {

		  }

		  inline UnsignedLong(uint64 val) : BaseTypeVariable<uint64>(val) {

		  }

		  inline UnsignedLong(const UnsignedLong& val) : BaseTypeVariable<uint64>(val) {

		  }

		  bool toString(String& str) {
			  str = String::valueOf(*this);

			  return true;
		  }

		  bool parseFromString(const String& str, int version = 0) {
			  *this = valueOf(str);

			  return true;
		  }

		  bool toBinaryStream(ObjectOutputStream* stream) {
			  stream->writeLong(BaseTypeVariable<uint64>::get());

			  return true;
		  }

		  bool parseFromBinaryStream(ObjectInputStream* stream) {
			  *this = stream->readLong();

			  return true;
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

		  static unsigned int hashCode(int64 value) {
			  return (unsigned int)(value ^ (value >> 32));
		  }
	  };

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*LONG_H_*/
