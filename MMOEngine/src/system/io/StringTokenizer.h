/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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
		int64 getLongToken();
		uint64 getUnsignedLongToken();
		float getFloatToken();

		void getStringToken(String& token);

		String getStringToken() {
			String val;

			getStringToken(val);

			return val;
		}

		void getStringToken(StringBuffer& token);

		void finalToken(String& s);

		String getRemainingString() {
			String str;

			finalToken(str);

			return str;
		}

		void shiftTokens(int count);

		bool hasMoreTokens() const;

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
