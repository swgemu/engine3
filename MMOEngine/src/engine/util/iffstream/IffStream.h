/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef IFFSTREAM_H_
#define IFFSTREAM_H_

#include "system/lang.h"
#include "engine/log/Logger.h"
#include "engine/util/ObjectFactory.h"
#include "chunks/Chunk.h"

namespace engine {
 namespace util {

	 class IffStream : public FileInputStream, public Logger {
		 uint32 dataSize;

		 String fileName;

		 Vector<Chunk*> mainChunks;
		 int lastOpenedChunk;

		 Chunk* openedChunk;

		 ObjectFactory<Chunk* (Chunk* par, uint32 id, uint32 size, char* data), uint32> chunkFactory;

	 public:
		 IffStream(const String& filename);
		 ~IffStream();

		 bool load();
		 void loadMainChunks(char* dataBuffer);

		 template <class K> void registerChunk(uint32 key) {
			 chunkFactory.registerObject<K>(key);
		 }

		 Chunk* createChunk(Chunk* par, uint32 id, uint32 size, char* data) {
			 Chunk* chunk = chunkFactory.createObject(id, par, id, size, data);

			 if (chunk == NULL)
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

		 void close();

	 private:
		 void loadChunks(Chunk* chunk, char* currOffs);
	 };
 }
}

using namespace engine::util;

#endif /*IFFSTREAM_H_*/
