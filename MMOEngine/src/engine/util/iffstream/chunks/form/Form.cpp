/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "Form.h"

#include "engine/util/iffstream/IffStream.h"

Form::Form(Chunk* parent, uint32 type, uint32 size, char* data) : Chunk(parent, type, size, data) {

}

Form::~Form() {
}

void Form::parseData() {
	formType = readNetInt();

	while (Stream::hasData())
		parseSubObjects();
}

void Form::parseSubObjects() {
	int type = readNetInt();

	int size = readNetInt();

	Chunk* chunk = iffStream->createChunk(this, type, size, offset);
	chunk->setIffStream(iffStream);
	chunk->parseData();

	addChunk(chunk);

	shiftOffset(size);
}
