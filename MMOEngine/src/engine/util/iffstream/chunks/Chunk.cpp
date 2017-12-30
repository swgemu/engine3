#include "Chunk.h"

Chunk::Chunk(Chunk* par, uint32 id, uint32 size, char* data) {
	writeStream(data, size);
	reset();

	parent = par;

	chunkID = id;
	chunkSize = size;

	lastOpenedSubChunk = -1;

	iffStream = nullptr;
}

Chunk::~Chunk() {
	while (subChunks.size() > 0)
		delete subChunks.remove(0);
}

void Chunk::parseData() {
}
