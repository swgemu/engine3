/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef OBJECTINPUTSTREAM_H_
#define OBJECTINPUTSTREAM_H_

#include "Stream.h"

#ifndef PLATFORM_WIN
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif


namespace sys {
  namespace io {

	class ObjectInputStream : public virtual Stream {
	public:
		ObjectInputStream() : Stream() {
		}

		ObjectInputStream(int initsize) : Stream(initsize) {
		}

		ObjectInputStream(int initsize, int capincr) : Stream(initsize, capincr) {
		}

		ObjectInputStream(char *buf, int len) : Stream(buf, len) {
		}

		virtual ~ObjectInputStream() {
		}

		// reading methods
		inline bool readBoolean() {
			shiftOffset(1);

			return *(uint8*)(offset-1);
		}

		inline uint8 readByte() {
			shiftOffset(1);

			return *(uint8*)(offset-1);
		}

		inline uint8 readByte(int offs) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 1 > end)
				throw StreamIndexOutOfBoundsException(this, offs + 1);

			return *(uint8*)(elementOffset);
		}

		inline int8 readSignedByte() {
			shiftOffset(1);

			return *(int8*)(offset-1);
		}

		inline int8 readSignedByte(int offs) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 1 > end)
				throw StreamIndexOutOfBoundsException(this, offs + 1);

			return *(int8*)(elementOffset);
		}

		inline uint16 readShort() {
			shiftOffset(2);

			return *(uint16*)(offset-2);
		}

		inline uint16 readShort(int offs) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 2 > end)
				throw StreamIndexOutOfBoundsException(this, offs + 2);

			return *(uint16*)(elementOffset);
		}

		inline int16 readSignedShort(int offs) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 2 > end)
				throw StreamIndexOutOfBoundsException(this, offs + 2);

			return *(int16*)(elementOffset);
		}

		inline int16 readSignedShort() {
			shiftOffset(2);

			return *(int16*)(offset-2);
		}

		inline uint32 readInt() {
			shiftOffset(4);

			return *(uint32*)(offset-4);
		}

		inline uint32 readNetInt() {
			return ntohl(readInt());
		}

		inline uint32 readInt(int offs) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 4 > end)
				throw StreamIndexOutOfBoundsException(this, offs + 4);

			return *(uint32*)(elementOffset);
		}

		inline int32 readSignedInt() {
			shiftOffset(4);

			return *(int32*)(offset-4);
		}

		inline int32 readSignedInt(int offs) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 4 > end)
				throw StreamIndexOutOfBoundsException(this, offs + 4);

			return *(int32*)(elementOffset);
		}

		inline uint64 readLong() {
			shiftOffset(8);

			return *(uint64*)(offset-8);
		}

		inline uint64 readLong(int offs) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 8 > end)
				throw StreamIndexOutOfBoundsException(this, offs + 8);

			return *(uint64*)(elementOffset);
		}

		inline int64 readSignedLong() {
			shiftOffset(8);

			return *(int64*)(offset-8);
		}

		inline int64 readSignedLong(int offs) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 8 > end)
				throw StreamIndexOutOfBoundsException(this, offs + 8);

			return *(int64*)(elementOffset);
		}

		inline float readFloat() {
			shiftOffset(4);

			return *(float*)(offset-4);
		}

		inline float readFloat(int offs) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 4 > end)
				throw StreamIndexOutOfBoundsException(this, offs + 4);

			return *(float*)(elementOffset);
		}

		inline double readDouble() {
			shiftOffset(sizeof(double));

			return *(double*)(offset-sizeof(double));
		}
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*OBJECTINPUTSTREAM_H_*/
