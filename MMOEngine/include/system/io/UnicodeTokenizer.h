/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef UNICODETOKENIZER_H_
#define UNICODETOKENIZER_H_

#include "../platform.h"

#include "../lang/Exception.h"

namespace sys {
  namespace io {

	class UnicodeTokenizer {
		unicode str;
		int index;
		unicode delimeter;

	public:
		UnicodeTokenizer(const unicode& u);
		UnicodeTokenizer(const string& s);
		UnicodeTokenizer(const char* ascii);
		UnicodeTokenizer(const char* ascii, int len);

		int getIntToken();
		uint32 getHexIntToken();
		uint64 getLongToken();
		float getFloatToken();

		void getStringToken(string& token);
		void getStringToken(stringstream& token);
		void getUnicodeToken(unicode& token);

		void finalToken(string& s);
		void finalToken(unicode& u);

		void shiftTokens(int count);

		bool hasMoreTokens() const;

		inline void setDelimeter(const string& del) {
			delimeter = del;
		}

		inline void setDelimiter(const unicode& del) {
			delimeter = del;
		}

		inline void setDelimiter(const char* ascii) {
			delimeter = ascii;
		}
	private:
		void nextToken(unicode& s);

	};

  } // namespace io
} // namespace sys

using namespace sys::io;


#endif /* UNICODETOKENIZER_H_ */
