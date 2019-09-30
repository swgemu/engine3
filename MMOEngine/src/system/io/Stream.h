/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#pragma once

#include "system/platform.h"

#include "system/util/ArrayList.h"

#include "StreamIndexOutOfBoundsException.h"

namespace sys {
  namespace io {

	class Stream : protected ArrayList<char> {
	protected:
		char *end, *offset;

	public:
		Stream();
		Stream(int initsize);
		Stream(int initsize, int capincr);
		Stream(const char *buf, int len);

		virtual ~Stream();

		Stream* clone(int startoffs = 0);

		void copy(Stream* stream, int startoffs = 0) const;

		void setSize(int len, bool copyContent = true);

		void extendSize(int len, bool copyContent = true);

		void setOffset(int offs);

		void shiftOffset(int offs);

		void clear();

		void reset();

		void removeLastBytes(int len);

		void removeRange(int fromIndex, int toIndex);

		void removeAll(int newSize = 10, int newIncrement = 5);

		// stream manipulation methods
		void writeStream(const char* buf, int len);
		void writeStream(const Stream* stream);
		void writeStream(const Stream* stream, int len);
		void writeStream(const Stream* stream, int len, int offs);

		void insertStream(const Stream* stream, int len, int offs);

		void readStream(char* buf, int len);
		void readStream(Stream* stream, int len);

		String toStringData() const;

		// getters
		inline int getOffset() const {
			return offset - elementData;
		}

		inline bool hasData() const {
			return offset < end;
		}

		inline char* getBuffer() {
			return elementData;
		}

		inline const char* getBuffer() const {
			return elementData;
		}

		inline int size() const {
			return end - elementData;
		}

		inline const char* begin() const {
			return elementData;
		}

		inline char* begin() {
			return elementData;
		}
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

