/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
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

		// writing methods
		inline void writeBoolean(bool val) {
			extendSize(1);
			*(uint8*)(offset-1) = val;
		}

		inline void writeByte(uint8 val) {
			extendSize(1);
			*(uint8*)(offset-1) = val;
		}
	
		inline void writeByte(int offs, uint8 val) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 1 > end)
				throw StreamIndexOutOfBoundsException(this, offs); 
			
			*(uint8*)elementOffset = val;
		}

		inline void writeSignedByte(int8 val) {
			extendSize(1);
	
			*(int8*)(offset-1) = val;
		}
	
		inline void writeShort(uint16 val) {
			extendSize(2);
	
			*(uint16*)(offset-2) = val;
		}
	
		inline void writeSignedShort(int16 val) {
			extendSize(2);
	
			*(int16*)(offset-2) = val;
		}
	
		inline void writeShort(int offs, uint16 val) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 2 > end)
				throw StreamIndexOutOfBoundsException(this, offs); 
	
			*(uint16*)elementOffset = val;
		}
	
		inline void writeInt(uint32 val) {
			extendSize(4);
	
			*(uint32*)(offset-4) = val;
		}
	
		inline void writeInt(int offs, uint32 val) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 4 > end)
				throw StreamIndexOutOfBoundsException(this, offs); 
	
			*(uint32*)elementOffset = val;
		}
	
		inline void writeSignedInt(int32 val) {
			extendSize(4);
	
			*(int32*)(offset-4) = val;
		}
	
		inline void writeLong(uint64 val) {
			extendSize(8);
	
			*(uint64*)(offset-8) = val;
		}
	
		inline void writeLong(int offs, uint64 val) {
			char* elementOffset = elementData + offs;
			if (elementOffset + 8 > end)
				throw StreamIndexOutOfBoundsException(this, offs); 
	
			*(uint64*)(elementOffset + offs) = val;
		}

		inline void writeSignedLong(int64 val) {
			extendSize(8);
	
			*(int64*)(offset-8) = val;
		}
	
		inline void writeFloat(float val) {
			extendSize(4);
	
			*(float*)(offset-4) = val;
		}
		
		inline void writeDouble(double val) {
			extendSize(sizeof(double));

			*(double*)(offset-sizeof(double)) = val;
		}

	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*OBJECTOUTPUTSTREAM_H_*/
