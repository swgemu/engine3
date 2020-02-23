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
#include <type_traits>

namespace sys {
  namespace lang {

	class UnicodeString : public Variable {
	public:
		using UnicodeCharType = uint16;
		using UnicodeCharSize = std::integral_constant<decltype(sizeof(UnicodeCharType)), sizeof(UnicodeCharType)>;

	private:
		UnicodeCharType* uString;
		uint32 count;

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
		void append(const UnicodeCharType* str, int len);

		UnicodeString replaceFirst(const UnicodeString& regex, const UnicodeString& replacement) const ;

		void clear();

		int indexOf(UnicodeCharType chr) const;
		int indexOf(const UnicodeString& str, int startPos = 0) const;

		UnicodeString subString(int beg, int end) const;
		UnicodeString substr(int beg, int length) const;

		const char* toCharArray() const;
		const UnicodeCharType* toWideCharArray() const;

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

		friend UnicodeString operator+(const UnicodeString& str1, const UnicodeString& str2) {
			return str1.concat(str2);
		}

		inline UnicodeCharType* begin() {
			return uString;
		}

		inline const UnicodeCharType* begin() const {
			return uString;
		}

		inline UnicodeCharType* end() {
			return uString + count;
		}

		inline const UnicodeCharType* end() const {
			return uString + count;
		}

		inline uint32 length() const {
			return count;
		}

	private:
		void create(const char* ascii, int len);
		void asciitowide(UnicodeCharType* unicode, const char* ascii, int len);

		void copy(UnicodeCharType* dest, const UnicodeCharType* src);
		void copy(UnicodeCharType* dest, const UnicodeCharType* src, int len);

		template<UnicodeCharSize::value_type CharTypeSize, std::enable_if_t<CharTypeSize == sizeof(std::u16string::value_type), int> = 0>
		String convertToString() const {
			std::wstring_convert<std::codecvt_utf8_utf16<std::u16string::value_type>, std::u16string::value_type> convertor;
			auto result = convertor.to_bytes(reinterpret_cast<const std::u16string::value_type*>(begin()), reinterpret_cast<const std::u16string::value_type*>(end()));

			return String(result.c_str(), result.length());
		}

		template<UnicodeCharSize::value_type CharTypeSize, std::enable_if_t<CharTypeSize == String::StringCharSize::value, int> = 0>
		String convertToString() const {
			return String(toCharArray(), count);
		}

	public:
		String toString() const {
			return convertToString<UnicodeCharSize::value>();
		}
	};


  } // namespace lang
} // namespace sys

using namespace sys::lang;

