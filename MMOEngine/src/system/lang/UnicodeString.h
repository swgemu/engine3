/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/platform.h"

#include "Variable.h"
#include "String.h"

#include <locale>
#include <codecvt>
#include <string>

namespace sys {
  namespace lang {

	class UnicodeString : public Variable {
		uint16* uString;
		int count;

	public:
		UnicodeString();
		UnicodeString(const char* ascii);
		UnicodeString(const String& ascii);
		UnicodeString(const char* ascii, int len);
		UnicodeString(const UnicodeString& str);
		UnicodeString(UnicodeString&& str);

		~UnicodeString();

		UnicodeString& operator=(const UnicodeString& str);
		UnicodeString& operator=(UnicodeString&& str);
		UnicodeString& operator=(const String& ascii);
		UnicodeString& operator=(const char* ascii);

		UnicodeString& operator+(const String& ascii);
		UnicodeString& operator+(const UnicodeString& str);
		UnicodeString& operator+(const char* ascii);

		int compareTo(const UnicodeString& str) const;

		unsigned char operator[](int index) const;

		UnicodeString concat(const UnicodeString& str) const;

		void append(const String& ascii);
		void append(const UnicodeString& uni);
		void append(const char* ascii);
		void append(const char* ascii, int len);
		void append(const unsigned short* str, int len);

		UnicodeString replaceFirst(const UnicodeString& regex, const UnicodeString& replacement) const ;

		void clear();

		int indexOf(unsigned short chr) const;
		int indexOf(const UnicodeString& str, int startPos = 0) const;

		UnicodeString subString(int beg, int end) const;
		UnicodeString substr(int beg, int length) const;

		const char* toCharArray() const;
		const uint16* toWideCharArray() const;

		bool toString(String& str) const;

		bool parseFromString(const String& str, int version = 0);

		bool toBinaryStream(sys::io::ObjectOutputStream* stream);
		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream);

		explicit operator String() const {
			return toString();
		}

		inline bool isEmpty() const {
			return count == 0;
		}

		bool operator== (const UnicodeString& str) const {
			return compareTo(str) == 0;
		}

		bool operator< (const UnicodeString& str) const {
			return compareTo(str) < 0;
		}

		bool operator> (const UnicodeString& str) const {
			return compareTo(str) > 0;
		}

		bool operator!= (const UnicodeString& str) const {
			return compareTo(str) != 0;
		}

	private:
		void create(const char* ascii, int len);
		void asciitowide(unsigned short* unicode, const char* ascii, int len);

		void copy(unsigned short* dest, const unsigned short* src);
		void copy(unsigned short* dest, const unsigned short* src, int len);

	public:
		inline int length() const {
			return count;
		}

		template <typename T>
		static String toUTF8(const std::basic_string<T, std::char_traits<T>, std::allocator<T>>& source) {
			std::wstring_convert<std::codecvt_utf8_utf16<T>, T> convertor;
			auto result = convertor.to_bytes(source);

			return String(result.c_str(), result.length());
		}

		String toString() const {
			std::u16string uStr;
			auto len = length();

			uStr.reserve(len);

			for (int i = 0; i < len; ++i) {
				uStr.push_back(uString[i]);
			}

			return toUTF8(uStr);
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

UnicodeString operator+(const UnicodeString& str1, const UnicodeString& str2);

