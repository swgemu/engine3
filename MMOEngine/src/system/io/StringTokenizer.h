/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STRINGTOKENIZER_H_
#define STRINGTOKENIZER_H_

#include "system/platform.h"

#include "system/lang/Exception.h"

#include "system/lang/String.h"
#include "system/lang/StringBuffer.h"

namespace sys {
  namespace io {

	class StringTokenizer {
		String str;

		int index;
		String delimiter;

	public:
		StringTokenizer(const String& s);

		int getIntToken();
		uint32 getHexIntToken();
		uint64 getLongToken();
		float getFloatToken();

		void getStringToken(String& token);
		void getStringToken(StringBuffer& token);

		void finalToken(String& s);

		void shiftTokens(int count);

		bool hasMoreTokens();

		inline void setDelimeter(const String& del) {
			delimiter = del;
		}

		inline void setDelimiter(const String& del) {
			delimiter = del;
		}
	private:
		void nextToken(String& s);
		void nextToken(StringBuffer& s);

	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*STRINGTOKENIZER_H_*/
