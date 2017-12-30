/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef UNICODE_H_
#define UNICODE_H_

#include "system/platform.h"

#include "Variable.h"

#ifndef PLATFORM_WIN
//#define wchar_t unsigned short
#endif

namespace sys {
  namespace lang {

	class String;

	class UnicodeString : public Variable {
		unsigned short* uString;
		int count;

	public:
		UnicodeString();
		UnicodeString(const char* ascii);
		UnicodeString(const String& ascii);
		UnicodeString(const char* ascii, int len);
		UnicodeString(const UnicodeString& str);

#ifdef CXX11_COMPILER
		UnicodeString(UnicodeString&& str) : Variable(), uString(str.uString), count(str.count) {
			str.uString = nullptr;
			str.count = 0;
		}
#endif

		~UnicodeString();

		UnicodeString& operator=(const UnicodeString& str);

#ifdef CXX11_COMPILER
		UnicodeString& operator=(UnicodeString&& str) {
			if (this == &str) {
				return *this;
			}

			if (uString != nullptr) {
				delete [] uString;
			}

			uString = str.uString;
			count = str.count;

			str.uString = nullptr;
			str.count = 0;

			return *this;
		}
#endif
		UnicodeString& operator=(const String& ascii);
		UnicodeString& operator=(const char* ascii);

		UnicodeString& operator+(const String& ascii);
		UnicodeString& operator+(const UnicodeString& str);
		UnicodeString& operator+(const char* ascii);

		int compareTo(const UnicodeString& str) const;

		char operator[](int index) const;

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
		const unsigned short* toWideCharArray() const;

		String toString() const;
		bool toString(String& str);

		bool parseFromString(const String& str, int version = 0);

		bool toBinaryStream(sys::io::ObjectOutputStream* stream);
		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream);

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
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

UnicodeString operator+(const UnicodeString& str1, const UnicodeString& str2);

#endif /*UNICODE_H_*/
