/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STRING_H_
#define STRING_H_

#include "../platform.h"
#include "Variable.h"

#define SSO_STRING

#ifdef SSO_STRING
#define SSO_SIZE 16
#endif

namespace sys {
  namespace lang {

	class String : public Variable {
	protected:
		int count;

#ifdef SSO_STRING
		union {
			char* value;
			char sso[SSO_SIZE];
		};
#else
		char* value;
#endif

	public:
		String();
		String(char* str);
		String(const char* str);
		String(const char* str, int len);
		String(const String& str);

		virtual ~String();

		String concat(char ch) const;
		String concat(int i) const;
		String concat(const char* str) const;
		String concat(const char* str, int len) const;
		String concat(const String& str) const;

		bool contains(const char* str) const ;
		bool contains(const String& str) const ;

		int compareTo(const char* str) const;
		int compareTo(const String& str) const;

		int indexOf(char ch) const ;
		int indexOf(char ch, int fromIndex) const ;
		int indexOf(const char* str) const ;
		int indexOf(const char* str, int fromIndex) const ;
		int indexOf(const String& str) const ;
		int indexOf(const String& str, int fromIndex) const ;

		int lastIndexOf(char ch) const ;
		int lastIndexOf(char ch, int fromIndex) const ;
		int lastIndexOf(const char* str) const ;
		int lastIndexOf(const char* str, int fromIndex) const ;
		int lastIndexOf(const String& str) const ;
		int lastIndexOf(const String& str, int fromIndex) const ;

		static String format(const char* format, ...);

		bool beginsWith(const char* str) const ;
		bool beginsWith(const String& str) const ;

		bool endsWith(const char* str) const ;
		bool endsWith(const String& str) const ;

		uint32 hashCode() const;

		static uint32 hashCode(const char* string);
		static uint32 hashCode(const char* string, int count);

		String subString(int beginIndex) const ;
		String subString(int beginIndex, int endIndex) const ;

		static String valueOf(int val);
		static String valueOf(uint32 val);
		static String valueOf(int64 val);
		static String valueOf(uint64 val);
		static String valueOf(float val);
		static String valueOf(double val);
		static String valueOf(void* val);

		static String hexvalueOf(int val) ;
		static String hexvalueOf(int64 val);

		String replaceFirst(const String& regex, const String& replacement) const ;
		String replaceAll(const String& regex, const String& replacement) const ;

		String toLowerCase() const ;

		String toUpperCase() const ;

		String trim() const;
		String escapeString() const ;

		String& operator= (const char* str);
		String& operator= (const String& str);

		bool operator== (const char* str) const {
			return compareTo(str) == 0;
		}

		bool operator== (const String& str) const {
			return compareTo(str) == 0;
		}

		bool operator< (const char* str) const {
			return compareTo(str) < 0;
		}

		bool operator< (const String& str) const {
			return compareTo(str) < 0;
		}

		bool operator> (const char* str) const {
			return compareTo(str) > 0;
		}

		bool operator> (const String& str) const {
			return compareTo(str) > 0;
		}

		bool operator!= (const char* str) const {
			return compareTo(str) != 0;
		}

		bool operator!= (const String& str) const {
			return compareTo(str) != 0;
		}

		bool toString(String& str);

		bool parseFromString(const String& str, int version = 0);

		bool toBinaryStream(sys::io::ObjectOutputStream* stream);
		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream);

		String& operator+= (char ch);
		String& operator+= (const char* str);
		String& operator+= (const String& str);

		char& operator[](int i);

		/*
		 * This allows nasty things like substracting ints from String because it casts to char* i.e:
		 * String test = "blabla";
		 * String mistake = test - 100; compiles fine and currupts memory :)
		operator char* () const {
			return value;
		}
		*/

		char charAt(int index) const;

		inline const char* toCharArray() const {
			return begin();
		}

		inline bool isEmpty() const {
			return count == 0;
		}

	protected:
		void create(const char* str, int len);

		void clear();

		static char* strrstr(const char* s, int slen, const char* t, int tlen);

		char* begin() const;
		char* end() const;

	public:
		// getters
		inline int length() const {
			return count;
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

bool operator==(char ch, const String& str2);
bool operator==(const String& str, char ch);
bool operator==(char* str1, const String& str2);
bool operator==(const char* str1, const String& str2);

bool operator!=(char ch, const String& str2);
bool operator!=(const String& str, char ch);
bool operator!=(char* str1, const String& str2);
bool operator!=(const char* str1, const String& str2);

String operator+(const String& str1, const String& str2);
String operator+(const char* str1, const String& str2);
String operator+(const String& str1, const char* str2);
String operator+(const String& str1, char ch);
String operator+(char ch, const String& str2);
String operator+(const String& str1, int i);

#endif /*STRING_H_*/
