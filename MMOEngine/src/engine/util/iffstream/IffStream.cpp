/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "IffStream.h"

#include "chunks/form/Form.h"
#include "chunks/data/Data.h"

#include "exceptions.h"

namespace IffStreamNS {
	template<typename T>
	T readPrimitiveFrom(char* elementData, int offs) {
		T val;
		memcpy(&val, elementData + offs, sizeof(T));

		return val;
	}
}

using namespace IffStreamNS;

IffStream::IffStream() {
	//fileName = filename;

	//data = str;

	openedChunk = nullptr;

	lastOpenedChunk = -1;

	dataSize = 0;

	setLoggingName("IffStream ");
}

IffStream::~IffStream() {
	/*if (file != nullptr && file->exists())
		close();*/

	mainChunks.forEach([](auto obj) {
		delete obj;
	});

	/*delete file;
	file = nullptr;*/
}

bool IffStream::parseChunks(sys::uint8* data, int size, const String& filename) {
	fileName = filename;

	setLoggingName("IffStream " + fileName);
	/*if (file == nullptr) {
		close();
		throw InvalidFileTypeException(this);
	}

	if (!file->exists()) {
		throw InvalidFileTypeException(this);
	}*/

	dataSize = size;//data->size();//file->size();

	if (dataSize < 8) {
		//close();
		throw InvalidFileTypeException(this);
	}

	// allocate memory:
	//char* dataBuffer = new char[dataSize];

	// read data as a block:
	//FileInputStream::read((sys::byte*)dataBuffer, dataSize);

	char* dataBuffer = (char*)data;//data->getBuffer();

	registerChunk<Form>(0x464F524D); //'FORM'
	registerChunk<Data>(0x44415441); //'DATA'

	loadMainChunks(dataBuffer);

	//delete [] dataBuffer;

	return true;
}

void IffStream::loadMainChunks(char* dataBuffer) {
	uint32 offset = 0;

	while (offset < (dataSize - 4)) {
		uint32 type = htonl(readPrimitiveFrom<uint32>(dataBuffer, offset));
		offset += 4;

		uint32 size = htonl(readPrimitiveFrom<uint32>(dataBuffer, offset));
		offset += 4;

		Chunk* chunk = createChunk(nullptr, type, size, dataBuffer + offset);
		chunk->setIffStream(this);
		chunk->parseData();

		mainChunks.add(chunk);

		offset += size;
	}
}

Chunk* IffStream::openForm(uint32 formType) {
	Chunk* chunkObject = nullptr;

	if (openedChunk == nullptr)
		chunkObject = mainChunks.get(++lastOpenedChunk);
	else
		chunkObject = openedChunk->getNextChunk();

	if (!chunkObject->isFORM()) {
		StringBuffer msg;
		msg << "Could not open chunk:[" << formType << "]!";

		error(msg.toString());

		return nullptr;
	}

	Form* formObject = (Form*)chunkObject;

	if (formObject->getFormType() != formType) {
		StringBuffer msg;
		msg << "Could not open chunk[" << formType << "]!";
		error(msg.toString());

		return nullptr;
	}

	openedChunk = formObject;

	return openedChunk;
}

int IffStream::getSubChunksNumber() {
	if (openedChunk == nullptr)
		return mainChunks.size();
	else
		return openedChunk->getChunksSize();
}

int IffStream::getRemainingSubChunksNumber() {
	if (openedChunk == nullptr) {
		return mainChunks.size() - (lastOpenedChunk + 1);
	} else {
		int result = openedChunk->getChunksSize() - (openedChunk->getLastOpenedChunkIdx() + 1);
		return result;
	}
}

int IffStream::getDataSize() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->getChunkSize();
}

uint32 IffStream::getFormType() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	if (!openedChunk->isFORM())
		throw InvalidChunkTypeException(this);

	Form* form = (Form*)openedChunk;

	uint32 type = form->getFormType();

	return type;
}

uint32 IffStream::getNextFormType() {
	Chunk* child;

	if (openedChunk == nullptr)
		child = mainChunks.get((lastOpenedChunk + 1));
	else {
		int nextChunk = openedChunk->getLastOpenedChunkIdx();
		child = openedChunk->getChunk(++nextChunk);
	}

	if (!child->isFORM())
		throw InvalidChunkTypeException(this);

	Form* form = (Form*) child;

	return form->getFormType();
}

bool IffStream::closeForm(uint32 formType) {
	closeChunk(formType);

	return true;
}

Chunk* IffStream::openChunk(uint32 chunk) {
	Chunk* chunkObject = nullptr;

	if (openedChunk == nullptr)
		chunkObject = mainChunks.get(++lastOpenedChunk);
	else
		chunkObject = openedChunk->getNextChunk();

	if (chunk != 0 && chunkObject->getChunkID() != chunk) {
		/*Chunk* test = nullptr;
		test->getChunkSize();*/

		StringBuffer msg;
		msg << "Could not open chunk:[" << chunk << "] expecting [" << chunkObject->getChunkID() << "]!";
		error(msg);
		return nullptr;
	}

	openedChunk = chunkObject;

	return openedChunk;
}

void IffStream::closeChunk(uint32 chunk) {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	openedChunk = openedChunk->getParent();

}

/*void IffStream::close() {
	FileInputStream::close();

	delete file;
	file = nullptr;
}*/

void IffStream::skipChunks(int num) {
	if (openedChunk != nullptr) {
		int chunks = openedChunk->getLastOpenedChunkIdx();
		chunks += num;
		openedChunk->setLastOpenedSubChunk(chunks);
	} else
		lastOpenedChunk += num;
}

void IffStream::getString(String& str) {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	openedChunk->readString(str);
}

float IffStream::getFloat() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->readFloat();
}

int IffStream::getInt() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->readSignedInt();
}

uint32 IffStream::getUnsignedInt() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->readInt();
}

uint8 IffStream::getByte() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->readByte();
}

uint64 IffStream::getUnsignedLong() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->readLong();
}

int64 IffStream::getLong() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->readSignedLong();
}

void IffStream::getBytes(int bytes, void* dest) {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	openedChunk->readStream((char*)dest, bytes);
}

Matrix4 IffStream::getMatrix3x4Transform() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->readMatrix3x4Transform();
}

Vector3 IffStream::getVector3() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->readVector3();
}

Vector4 IffStream::getVector4() {
	if (openedChunk == nullptr)
		throw NoOpenedChunkException(this);

	return openedChunk->readVector4();
}
