/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef UNICODE_H_
#define UNICODE_H_

#include "../platform.h"

#include "Variable.h"

#ifndef PLATFORM_WIN
//#define wchar_t unsigned short
#endif

namespace sys {
  namespace lang {

	class String;

	class UnicodeString : public virtual Variable {
		unsigned short* uString;
		int count;

	public:
		UnicodeString();
		UnicodeString(const char* ascii);
		UnicodeString(const String& ascii);
		UnicodeString(const char* ascii, int len);
		UnicodeString(const UnicodeString& str);

		~UnicodeString();

		UnicodeString& operator=(const UnicodeString& str);
		UnicodeString& operator=(const String& ascii);
		UnicodeString& operator=(const char* ascii);

		UnicodeString& operator+(const String& ascii);
		UnicodeString& operator+(const UnicodeString& str);
		UnicodeString& operator+(const char* ascii);

		char operator[](int index) const;

		void append(const String& ascii);
		void append(const UnicodeString& uni);
		void append(const char* ascii);
		void append(const char* ascii, int len);
		void append(const unsigned short* str, int len);

		void clear();

		int indexOf(unsigned short chr) const;
		int indexOf(const UnicodeString& str, int startPos = 0) const;

		UnicodeString subString(int beg, int end) const;
		UnicodeString substr(int beg, int length) const;

		const unsigned short* toWideCharArray() const;

		String toString() const;
		bool toString(String& str);

		bool parseFromString(const String& str, int version = 0);


		bool toBinaryStream(sys::io::ObjectOutputStream* stream);
		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream);

		inline bool isEmpty() const {
			return count == 0;
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

#endif /*UNICODE_H_*/
