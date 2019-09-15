/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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

		template<typename T>
		T readPrimitive() {
			constexpr auto typeSize = sizeof(T);

			shiftOffset(typeSize);

			T val;
			memcpy(&val, offset - typeSize, typeSize);

			return val;
		}

		template<typename T>
		T readPrimitiveFrom(int offs) {
			constexpr auto typeSize = sizeof(T);

			auto elementOffset = elementData + offs;

			if (elementOffset + typeSize > end)
				throw StreamIndexOutOfBoundsException(this, offs + typeSize);

			T val;
			memcpy(&val, elementOffset, typeSize);

			return val;
		}

		// reading methods
		inline bool readBoolean() {
			return readPrimitive<uint8>();
		}

		inline uint8 readByte() {
			return readPrimitive<uint8>();
		}

		inline uint8 readByte(int offs) {
			return readPrimitiveFrom<uint8>(offs);
		}

		inline int8 readSignedByte() {
			return readPrimitive<int8>();
		}

		inline int8 readSignedByte(int offs) {
			return readPrimitiveFrom<int8>(offs);
		}

		inline uint16 readShort() {
			return readPrimitive<uint16>();
		}

		inline uint16 readShort(int offs) {
			return readPrimitiveFrom<uint16>(offs);
		}

		inline int16 readSignedShort(int offs) {
			return readPrimitiveFrom<int16>(offs);
		}

		inline int16 readSignedShort() {
			return readPrimitive<int16>();
		}

		inline uint32 readInt() {
			return readPrimitive<uint32>();
		}

		inline uint32 readNetInt() {
			return ntohl(readInt());
		}

		inline uint32 readInt(int offs) {
			return readPrimitiveFrom<uint32>(offs);
		}

		inline int32 readSignedInt() {
			return readPrimitive<int32>();
		}

		inline int32 readSignedInt(int offs) {
			return readPrimitiveFrom<int32>(offs);
		}

		inline uint64 readLong() {
			return readPrimitive<uint64>();
		}

		inline uint64 readLong(int offs) {
			return readPrimitiveFrom<uint64>(offs);
		}

		inline int64 readSignedLong() {
			return readPrimitive<int64>();
		}

		inline int64 readSignedLong(int offs) {
			return readPrimitiveFrom<int64>(offs);
		}

		inline float readFloat() {
			return readPrimitive<float>();
		}

		inline float readFloat(int offs) {
			return readPrimitiveFrom<float>(offs);
		}

		inline double readDouble() {
			return readPrimitive<double>();
		}
	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*OBJECTINPUTSTREAM_H_*/
