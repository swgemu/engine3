/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef STREAM_H_
#define STREAM_H_

#include "system/platform.h"

#include "system/util/ArrayList.h"

#include "StreamIndexOutOfBoundsException.h"

namespace sys {
  namespace io {

	class Stream : public ArrayList<char> {
	protected:
		char *end, *offset;
		
	public:
		Stream();	
		Stream(int initsize);
		Stream(int initsize, int capincr);
		Stream(char *buf, int len);
		
		virtual ~Stream();

		Stream* clone(int startoffs = 0);
		
		void copy(Stream* stream, int startoffs = 0);
		
		void setSize(int len, bool copyContent = true);
		
		void extendSize(int len, bool copyContent = true);
		
		void setOffset(int offs);
	
		void shiftOffset(int offs);
	
		void clear();
	
		void reset();
	
		void removeLastBytes(int len);

		void removeRange(int fromIndex, int toIndex);

		// stream manipulation methods
		void writeStream(const char* buf, int len);
		void writeStream(Stream* stream);
		void writeStream(Stream* stream, int len);
		void writeStream(Stream* stream, int len, int offs);

		void insertStream(Stream* stream, int len, int offs);

		void readStream(char* buf, int len);
		void readStream(Stream* stream, int len);
		
		String toStringData();

		// getters
		inline int getOffset() {
			return offset - elementData;
		}
	
		inline bool hasData() {
			return offset < end;
		}
	
		inline char* getBuffer() {
			return elementData;
		}
		
		inline int size() {
			return end - elementData;
		}
		
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*STREAM_H_*/
