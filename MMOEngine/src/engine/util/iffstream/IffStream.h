/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef IFFSTREAM_H_
#define IFFSTREAM_H_

#include "system/lang.h"
#include "engine/log/Logger.h"
#include "engine/util/ObjectFactory.h"
#include "chunks/Chunk.h"

namespace engine {
 namespace util {

	 class IffStream : public Logger {
		 uint32 dataSize;

		 String fileName;

		 //ObjectInputStream* data;

		 Vector<Chunk*> mainChunks;
		 int lastOpenedChunk;

		 Chunk* openedChunk;

		 ObjectFactory<Chunk* (Chunk* par, uint32 id, uint32 size, char* data), uint32> chunkFactory;

	 public:
		 IffStream();
		 //IffStream(const String& filename);
		 ~IffStream();

		 bool parseChunks(sys::uint8* data, int size, const String& filename);

		 template <class K> void registerChunk(uint32 key) {
			 chunkFactory.registerObject<K>(key);
		 }

		 Chunk* createChunk(Chunk* par, uint32 id, uint32 size, char* data) {
			 Chunk* chunk = chunkFactory.createObject(id, par, id, size, data);

			 if (chunk == nullptr)
				 chunk = new Chunk(par, id, size, data);

			 return chunk;
		 }

		 Chunk* openForm(uint32 formType);
		 bool closeForm(uint32 formType);

		 int getDataSize();

		 int getSubChunksNumber();
		 int getRemainingSubChunksNumber();

		 Chunk* openChunk(uint32 chunk = 0);
		 void closeChunk(uint32 chunk = 0);

		 void skipChunks(int num = 1);

		 inline String& getFileName() {
			 return fileName;
		 }

		 inline const String& getFileName() const {
			 return fileName;
		 }

		 uint32 getFormType();
		 uint32 getNextFormType();

		 // io operations
		 void getString(String& str);

		 float getFloat();
		 int getInt();
		 uint32 getUnsignedInt();

		 uint8 getByte();
		 uint64 getUnsignedLong();
		 int64 getLong();

		 void getBytes(int bytes, void* dest);

		 Matrix4 getMatrix3x4Transform();
		 Vector3 getVector3();
		 Vector4 getVector4();

		 //void close();

	 private:
		 void loadMainChunks(char* dataBuffer);
	 };
 }
}

using namespace engine::util;

#endif /*IFFSTREAM_H_*/
