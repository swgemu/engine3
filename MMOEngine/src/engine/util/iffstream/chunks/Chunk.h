/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef CHUNK2_H_
#define CHUNK2_H_

#include "system/lang.h"

#include "engine/util/u3d/Matrix4.h"
#include "engine/util/u3d/Vector3.h"
#include "engine/util/u3d/Vector4.h"

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

		String readString() {
			String val;
			readString(val);

			return val;
		}

		Matrix4 readMatrix3x4Transform() {
			Matrix4 mat;

			for (int x = 0; x < 3; ++x) {
				for (int y = 0; y < 4; ++y) {
					mat[x][y] = readFloat();
				}
			}

			return mat;
		}

		Vector3 readVector3() {
			Vector3 vec;

			for (int i = 0; i < 3; ++i) {
				vec[i] = readFloat();
			}

			return vec;
		}

		Vector4 readVector4() {
			Vector4 vec;

			for (int i = 0; i < 4; ++i) {
				vec[i] = readFloat();
			}

			return vec;
		}

	};

 }
}

using namespace engine::util;

#endif /*CHUNK_H_*/
