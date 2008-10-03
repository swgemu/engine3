/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STRINGTOKENIZER_H_
#define STRINGTOKENIZER_H_

#include "../platform.h"

#include "../lang/Exception.h"

namespace sys {
  namespace io { 

	class StringTokenizer {
		string str;
		string::size_type index;
		string delimeter;
		
	public:
		StringTokenizer(const string& s);
	
		int getIntToken();
		uint32 getHexIntToken();
		uint64 getLongToken();
		float getFloatToken();
		
		void getStringToken(string& token);
		void getStringToken(stringstream& token);
	
		void finalToken(string& s);

		void shiftTokens(int count);
		
		bool hasMoreTokens();

		inline void setDelimeter(const string& del) {
			delimeter = del;
		}

	private:
		void nextToken(string& s);
		void nextToken(stringstream& s);
		
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*STRINGTOKENIZER_H_*/
