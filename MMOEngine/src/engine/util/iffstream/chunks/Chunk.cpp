/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
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
	subChunks.forEach([](auto chunk) {
		delete chunk;
	});
}

void Chunk::parseData() {
}
