/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef OBJECTOUTPUTSTREAM_H_
#define OBJECTOUTPUTSTREAM_H_

#include "Stream.h"

namespace sys {
  namespace io {

	class ObjectOutputStream : public virtual Stream {
	public:
		ObjectOutputStream() : Stream() {
		}

		ObjectOutputStream(int initsize) : Stream(initsize) {
		}

		ObjectOutputStream(int initsize, int capincr) : Stream(initsize, capincr) {
		}

		ObjectOutputStream(char *buf, int len) : Stream(buf, len) {
		}

		virtual ~ObjectOutputStream() {
		}

		template<typename T>
		void writePrimitive(const T& val) {
			constexpr auto typeSize = sizeof(T);

			extendSize(typeSize);

			memcpy(offset - typeSize, &val, typeSize);
		}

		template<typename T>
		void writePrimitiveTo(int offs, const T& val) {
			constexpr auto typeSize = sizeof(T);

			char* elementOffset = elementData + offs;
			if (elementOffset + typeSize > end)
				throw StreamIndexOutOfBoundsException(this, offs);

			memcpy(elementOffset, &val, typeSize);
		}

		// writing methods
		inline void writeBoolean(bool val) {
			writePrimitive<uint8>(val);
		}

		inline void writeByte(uint8 val) {
			writePrimitive(val);
		}

		inline void writeByte(int offs, uint8 val) {
			writePrimitiveTo(offs, val);
		}

		inline void writeSignedByte(int8 val) {
			writePrimitive(val);
		}

		inline void writeShort(uint16 val) {
			writePrimitive(val);
		}

		inline void writeSignedShort(int16 val) {
			writePrimitive(val);
		}

		inline void writeShort(int offs, uint16 val) {
			writePrimitiveTo(offs, val);
		}

		inline void writeInt(uint32 val) {
			writePrimitive(val);
		}

		inline void writeInt(int offs, uint32 val) {
			writePrimitiveTo(offs, val);
		}

		inline void writeSignedInt(int32 val) {
			writePrimitive(val);
		}

		inline void writeLong(uint64 val) {
			writePrimitive(val);
		}

		inline void writeLong(int offs, uint64 val) {
			writePrimitiveTo(offs, val);
		}

		inline void writeSignedLong(int64 val) {
			writePrimitive(val);
		}

		inline void writeFloat(float val) {
			writePrimitive(val);
		}

		inline void writeDouble(double val) {
			writePrimitive(val);
		}

	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*OBJECTOUTPUTSTREAM_H_*/
