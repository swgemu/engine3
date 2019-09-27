/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/platform.h"
#include "Variable.h"
#include "ConstString.h"

#include <cstdarg>

#define SSO_STRING

//#define STRING_INHERIT_VARIABLE

namespace sys {
  namespace lang {

	//separating from String to make gdb not print them with every instance
	class StringHashCodeTable {
	public:
#ifdef CXX11_COMPILER
		static constexpr uint32 crctable[256] = {
				0x0000000,
				0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B,
				0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6,
				0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
				0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC,
				0x5BD4B01B, 0x569796C2, 0x52568B75, 0x6A1936C8, 0x6ED82B7F,
				0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A,
				0x745E66CD, 0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
				0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58,
				0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033,
				0xA4AD16EA, 0xA06C0B5D, 0xD4326D90, 0xD0F37027, 0xDDB056FE,
				0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
				0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4,
				0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 0x34867077, 0x30476DC0,
				0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5,
				0x2AC12072, 0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16,
				0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA, 0x7897AB07,
				0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C,
				0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1,
				0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
				0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B,
				0xBB60ADFC, 0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698,
				0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D,
				0x94EA7B2A, 0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E,
				0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2, 0xC6BCF05F,
				0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34,
				0xDC3ABDED, 0xD8FBA05A, 0x690CE0EE, 0x6DCDFD59, 0x608EDB80,
				0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
				0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A,
				0x58C1663D, 0x558240E4, 0x51435D53, 0x251D3B9E, 0x21DC2629,
				0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C,
				0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
				0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 0xF12F560E,
				0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65,
				0xEBA91BBC, 0xEF68060B, 0xD727BBB6, 0xD3E6A601, 0xDEA580D8,
				0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
				0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2,
				0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71,
				0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74,
				0x857130C3, 0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640,
				0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 0x7B827D21,
				0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A,
				0x61043093, 0x65C52D24, 0x119B4BE9, 0x155A565E, 0x18197087,
				0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
				0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D,
				0x2056CD3A, 0x2D15EBE3, 0x29D4F654, 0xC5A92679, 0xC1683BCE,
				0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB,
				0xDBEE767C, 0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18,
				0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4, 0x89B8FD09,
				0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662,
				0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF,
				0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4,
		};
#else
		static const uint32 crctable[256];
#endif
	};

#ifdef STRING_INHERIT_VARIABLE
	class String : public Variable {
#else
	class String {
#endif
	protected:
		static const int constexpr SSO_SIZE = int(sizeof(char*) * 2);

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
		String(const char* str);
		String(const char* str, int len);
		String(const String& str);
		String(const ConstString& str);

		static const int constexpr npos = -1;

#ifdef CXX11_COMPILER
		String(String&& str);
#endif

		~String();

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

		//sprintf format
		template<std::size_t BufferSize = 1024>
		static String format(const char* format, ...) {
			char buffer[BufferSize]; //VLA is not officially supported in cpp

			va_list args;
			va_start(args, format);

			//Notice that only when this returned value is non-negative and less than n, the string has been completely written.
			int res = vsnprintf(buffer, sizeof(buffer), format, args);

			va_end(args);

			E3_ASSERT(res >= 0 && "String::format formatting error");
			E3_ASSERT(static_cast<std::size_t>(res) < sizeof(buffer) && "data could not fit in String::format buffer");

			return String(buffer, res);
		}

		bool beginsWith(const char* str) const ;
		bool beginsWith(const String& str) const ;

		bool endsWith(const char* str) const ;
		bool endsWith(const String& str) const ;

		uint32 hashCode() const;

#ifdef CXX11_COMPILER
		static constexpr uint32 hashCode(const char* string, uint32 startCRC = 0xFFFFFFFF) {
			return *string ?
					hashCode(string + 1, StringHashCodeTable::crctable[((startCRC >> 24) ^ (byte)(*string)) & 0xFF] ^ (startCRC << 8))
					: ~startCRC;
		}
#else
		static uint32 hashCode(const char* string, uint32 startCRC = 0xFFFFFFFF) {
			uint32 CRC = startCRC;

			for (; *string; ++string) {
				CRC = StringHashCodeTable::crctable[((CRC >> 24) ^ static_cast<byte>(*string)) & 0xFF] ^ (CRC << 8);
			}

			return ~CRC;
		}
#endif

		static uint32 hashCode(const String& str);

		String subString(int beginIndex) const ;
		String subString(int beginIndex, int endIndex) const ;

		static String valueOf(int val);
		static String valueOf(uint32 val);
		static String valueOf(long val);
		static String valueOf(int64 val);
		static String valueOf(uint64 val);
		static String valueOf(float val);
		static String valueOf(double val);
		static String valueOf(const void* val);
		static String valueOf(std::size_t val);
		static String valueOf(bool value);

		static String hexvalueOf(int val);
		static String hexvalueOf(long val);
		static String hexvalueOf(int64 val);
		static String hexvalueOf(uint32 val);
		static String hexvalueOf(uint64 val);

		static const String& valueOf(const String& str) {
			return str;
		}

		String replaceFirst(const String& regex, const String& replacement) const ;
		String replaceAll(const String& regex, const String& replacement) const ;

		String toLowerCase() const ;
		void changeToLowerCase();

		String toUpperCase() const ;
		void changeToUpperCase();

		String trim() const;
		String escapeString() const ;

		String& operator= (const char* str);
		String& operator= (const String& str);

#ifdef CXX11_COMPILER
		String& operator=(String&& str);
#endif

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
		const char& operator[](int i) const;

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

		inline const char* begin() const {
#ifdef SSO_STRING
			return count < SSO_SIZE ? (const char*) sso : (const char*) value;
#else
			return value;
#endif
		}

		inline const char* end() const {
			return begin() + count;
		}

		inline char* begin() {
#ifdef SSO_STRING
			return count < SSO_SIZE ? (char*) sso : (char*) value;
#else
			return value;
#endif
		}

		inline char* end() {
			return begin() + count;
		}

	public:
		// getters
		inline int length() const {
			return count;
		}
	};

#ifdef CXX11_COMPILER
	template<uint32 M>
	class StringHashCodeHelper {
	public:
		const uint32 value;

		constexpr StringHashCodeHelper() : value(M) {
		}
	};
#endif

#ifndef STRING_INHERIT_VARIABLE
	class SerializableString : public String, public Variable {
#else
	class SerializableString : public String {
#endif
	public:
		bool toBinaryStream(sys::io::ObjectOutputStream* stream) {
			return String::toBinaryStream(stream);
		}

		bool parseFromBinaryStream(sys::io::ObjectInputStream* stream) {
			return String::parseFromBinaryStream(stream);
		}

		SerializableString& operator=(const String& s) {
			String::operator=(s);

			return *this;
		}

		SerializableString& operator=(const SerializableString& s) {
			if (this == &s)
				return *this;

			String::operator=(s);

			return *this;
		}
	};

	} // namespace lang
} // namespace sys

using namespace sys::lang;

bool operator==(char ch, const String& str2);
bool operator==(const String& str, char ch);
bool operator==(const char* str1, const String& str2);

bool operator!=(char ch, const String& str2);
bool operator!=(const String& str, char ch);
bool operator!=(const char* str1, const String& str2);

String operator+(const String& str1, const String& str2);
String operator+(const char* str1, const String& str2);
String operator+(const String& str1, const char* str2);
String operator+(const String& str1, char ch);
String operator+(char ch, const String& str2);
String operator+(const String& str1, int i);

#ifdef CXX11_COMPILER
//forces the hash code to be calculated at compile time of a const string
#define STRING_HASHCODE(a) StringHashCodeHelper<String::hashCode(a)>().value
#else
#define STRING_HASHCODE(a) String::hashCode(a)
#endif

constexpr uint32 operator "" _hashCode(char const* str, std::size_t s) {
	return String::hashCode(str);
}

