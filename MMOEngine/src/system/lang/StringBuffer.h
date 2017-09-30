#ifndef STRINGBUFFER_H_
#define STRINGBUFFER_H_

#include "system/platform.h"

#include "String.h"

#include "system/util/ArrayList.h"

namespace sys {
  namespace lang {

  	enum StreamFlags {
		SF_none = 0L,
  		SF_dec = 1L << 0,
  		SF_hex = 1L << 1,
  		SF_uppercase = 1L << 2,
  		SF_endl = 1L << 3
  	};

  	const StreamFlags dec = SF_dec;
  	const StreamFlags hex = SF_hex;
  	const StreamFlags uppercase = SF_uppercase;
  	const StreamFlags endl = SF_endl;

	class StringBuffer : private ArrayList<char> {
		StreamFlags streamFlags;

	public:
		StringBuffer();
		StringBuffer(const String& str);
		StringBuffer(const StringBuffer& str);

		virtual ~StringBuffer();

		StringBuffer& append(char ch);
		StringBuffer& append(int val);
		StringBuffer& append(uint32 val);
		StringBuffer& append(long val);
		StringBuffer& append(int64 val);
		StringBuffer& append(uint64 val);
		StringBuffer& append(float val);
		StringBuffer& append(double val);
		StringBuffer& append(void* val);
		StringBuffer& append(const char* str);
		StringBuffer& append(const char* str, int len);
		StringBuffer& append(const String& str);

		void deleteRange(int start, int end);

		void deleteAll();

		int indexOf(char ch) const ;
		int indexOf(char ch, int fromIndex) const ;
		int indexOf(const String& str) const ;
		int indexOf(const String& str, int fromIndex) const ;
		int indexOf(const String& regexString, int& resultStart, int& resultEnd, int fromIndex = 0) const;

		StringBuffer& insert(int offset, char ch);
		StringBuffer& insert(int offset, int val);
		StringBuffer& insert(int offset, uint32 val);
		StringBuffer& insert(int offset, long val);
		StringBuffer& insert(int offset, int64 val);
		StringBuffer& insert(int offset, uint64 val);
		StringBuffer& insert(int offset, float val);
		StringBuffer& insert(int offset, double val);
		StringBuffer& insert(int offset, void* val);
		StringBuffer& insert(int offset, const char* str);
		StringBuffer& insert(int offset, const char* str, int len);
		StringBuffer& insert(int offset, const String& str);

		StringBuffer& replace(int start, int end, const char* str);
		StringBuffer& replace(int start, int end, const String& str);

		String toString() const ;

		void toString(String& str) const;

		// getters
		inline bool doHex() {
			return streamFlags & SF_hex;
		}

		inline bool doUpperCase() {
			return streamFlags & SF_uppercase;
		}

		inline int length() const {
			return ArrayList<char>::size();
		}

		inline const char* getBuffer() const {
			return ArrayList<char>::begin();
		}

		StringBuffer& operator<< (char ch);
		StringBuffer& operator<< (int val);
		StringBuffer& operator<< (uint32 val);
		StringBuffer& operator<< (long val);
		StringBuffer& operator<< (int64 val);
		StringBuffer& operator<< (uint64 val);
		StringBuffer& operator<< (float val);
		StringBuffer& operator<< (double val);
		StringBuffer& operator<< (void* val);
		StringBuffer& operator<< (const char* str);
		StringBuffer& operator<< (const String& str);
		StringBuffer& operator<< (const StreamFlags flags);
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*STRINGBUFFER_H_*/
