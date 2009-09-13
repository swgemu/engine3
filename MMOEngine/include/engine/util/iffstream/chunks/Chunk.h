/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef CHUNK2_H_
#define CHUNK2_H_

#include "system/lang.h"

namespace engine {
 namespace util {

	class IffStream;

	class Chunk : public ObjectInputStream {
	protected:
		IffStream* iffStream;
		Chunk* parent;

		uint32 chunkID;
		int chunkSize;

		Vector<Chunk*> subChunks;
		int lastOpenedSubChunk;

	public:
		const static uint32 FORM =  0x464F524D; //'FORM';
		const static uint32 DATA =  0x44415441; //'DATA';
		const static uint32 LIST = 0x4C495354; //'LIST';
		const static uint32 CAT = 0x43415420; //'CAT ';

	public:
		Chunk(Chunk* par, uint32 id, uint32 size, char* data);
		virtual ~Chunk();

		virtual void parseData();

		inline void setIffStream(IffStream* iff) {
			iffStream = iff;
		}

		inline IffStream* getIffStream() {
			return iffStream;
		}

		inline void setLastOpenedSubChunk(int value) {
			lastOpenedSubChunk = value;
		}

		inline void getChildren(Vector<Chunk*>& child) {
			child = subChunks;
		}

		inline bool isFORM() {
			return chunkID == FORM;
		}

		inline bool isDATA() {
			return chunkID == DATA;
		}

		inline Chunk* getParent() {
			return parent;
		}

		void addChunk(Chunk* chunk) {
			subChunks.add(chunk);
		}

		Chunk* getChunk(int idx) {
			return subChunks.get(idx);
		}

		Chunk* getNextChunk() {
			return subChunks.get(++lastOpenedSubChunk);
		}

		uint32 getChunksSize() {
			return subChunks.size();
		}

		inline int getLastOpenedChunkIdx() {
			return lastOpenedSubChunk;
		}

		inline uint32 getChunkID() {
			return chunkID;
		}

		inline int getChunkSize() {
			return chunkSize;
		}

		void readString(String& str) {
			StringBuffer stream;
			char byte;

			while ((byte = readSignedByte()) != 0)
				stream << byte;

			str = stream.toString();
		}

	};

 }
}

using namespace engine::util;

#endif /*CHUNK_H_*/
