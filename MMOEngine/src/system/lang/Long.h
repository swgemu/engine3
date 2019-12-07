/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include <cstdlib>

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

		  bool toString(String& str) const {
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

		  static uint32 hashCode(uint64 value) {
			  uint64 key = value;

			  key = (~key) + (key << 18); // key = (key << 18) - key - 1;
			  key =   key  ^ (key >> 31);
			  key = key * 21;             // key = (key + (key << 2)) + (key << 4);
			  key = key ^ (key >> 11);
			  key = key + (key << 6);
			  key = key ^ (key >> 22);

			  return (uint32) key;
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
			  int written = snprintf(buf, 32, "%I64d", val);
#else
			  int written = snprintf(buf, 32, "%lld", val);
#endif
			  E3_ASSERT(written >= 0 && written < 32);

			  str = String(buf, written);
		  }

		  static String toString(uint64 val) {
			  String str;

			  toString(str, val);

			  return str;
		  }

		  static void toString(String& str, uint64 val) {
			  char buf[32];

#ifdef PLATFORM_WIN
			  int written = snprintf(buf, 32, "%I64u", val);
#else
			  int written = snprintf(buf, 32, "%llu", val);
#endif
			  E3_ASSERT(written >= 0 && written < 32);

			  str = String(buf, written);
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

		  bool toString(String& str) const {
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

		  static uint64 hexvalueOf(const String& str) {
		  	return std::strtoull(str.toCharArray(), nullptr, 16);
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

		  static uint32 hashCode(int64 value) {
			  return Long::hashCode(value);
		  }
	  };

  } // namespace lang
} // namespace sys

using namespace sys::lang;

