#include "IffStream.h"

#include "chunks/form/Form.h"
#include "chunks/data/Data.h"

#include "exceptions.h"

IffStream::IffStream() {
	//fileName = filename;

	//data = str;

	openedChunk = NULL;

	lastOpenedChunk = -1;

	dataSize = 0;

	setLoggingName("IffStream ");
}

IffStream::~IffStream() {
	/*if (file != NULL && file->exists())
		close();*/

	while (mainChunks.size() > 0)
		delete mainChunks.remove(0);

	/*delete file;
	file = NULL;*/
}

bool IffStream::parseChunks(sys::uint8* data, int size, const String& filename) {
	fileName = filename;

	setLoggingName("IffStream " + fileName);
	/*if (file == NULL) {
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
		uint32 type = htonl(*(uint32*)(dataBuffer + offset));
		offset += 4;

		uint32 size = htonl(*(uint32*)(dataBuffer + offset));
		offset += 4;

		Chunk* chunk = createChunk(NULL, type, size, dataBuffer + offset);
		chunk->setIffStream(this);
		chunk->parseData();

		mainChunks.add(chunk);

		offset += size;
	}
}

Chunk* IffStream::openForm(uint32 formType) {
	Chunk* chunkObject = NULL;

	if (openedChunk == NULL)
		chunkObject = mainChunks.get(++lastOpenedChunk);
	else
		chunkObject = openedChunk->getNextChunk();

	if (!chunkObject->isFORM()) {
		StringBuffer msg;
		msg << "Could not open chunk:[" << formType << "]!";

		error(msg.toString());

		return NULL;
	}

	Form* formObject = (Form*)chunkObject;

	if (formObject->getFormType() != formType) {
		StringBuffer msg;
		msg << "Could not open chunk[" << formType << "]!";
		error(msg.toString());

		return NULL;
	}

	openedChunk = formObject;

	return openedChunk;
}

int IffStream::getSubChunksNumber() {
	if (openedChunk == NULL)
		return mainChunks.size();
	else
		return openedChunk->getChunksSize();
}

int IffStream::getRemainingSubChunksNumber() {
	if (openedChunk == NULL) {
		return mainChunks.size() - (lastOpenedChunk + 1);
	} else {
		int result = openedChunk->getChunksSize() - (openedChunk->getLastOpenedChunkIdx() + 1);
		return result;
	}
}

int IffStream::getDataSize() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->getChunkSize();
}

uint32 IffStream::getFormType() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	if (!openedChunk->isFORM())
		throw InvalidChunkTypeException(this);

	Form* form = (Form*)openedChunk;

	uint32 type = form->getFormType();

	return type;
}

uint32 IffStream::getNextFormType() {
	Chunk* child;

	if (openedChunk == NULL)
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
	Chunk* chunkObject = NULL;

	if (openedChunk == NULL)
		chunkObject = mainChunks.get(++lastOpenedChunk);
	else
		chunkObject = openedChunk->getNextChunk();

	if (chunk != 0 && chunkObject->getChunkID() != chunk) {
		/*Chunk* test = NULL;
		test->getChunkSize();*/

		StringBuffer msg;
		msg << "Could not open chunk:[" << chunk << "] expecting [" << chunkObject->getChunkID() << "]!";
		error(msg);
		return NULL;
	}

	openedChunk = chunkObject;

	return openedChunk;
}

void IffStream::closeChunk(uint32 chunk) {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	openedChunk = openedChunk->getParent();

}

/*void IffStream::close() {
	FileInputStream::close();

	delete file;
	file = NULL;
}*/

void IffStream::skipChunks(int num) {
	if (openedChunk != NULL) {
		int chunks = openedChunk->getLastOpenedChunkIdx();
		chunks += num;
		openedChunk->setLastOpenedSubChunk(chunks);
	} else
		lastOpenedChunk += num;
}

void IffStream::getString(String& str) {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	openedChunk->readString(str);
}

float IffStream::getFloat() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->readFloat();
}

int IffStream::getInt() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->readSignedInt();
}

uint32 IffStream::getUnsignedInt() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->readInt();
}

uint8 IffStream::getByte() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->readByte();
}

uint64 IffStream::getUnsignedLong() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->readLong();
}

int64 IffStream::getLong() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->readSignedLong();
}

void IffStream::getBytes(int bytes, void* dest) {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	openedChunk->readStream((char*)dest, bytes);
}

Matrix4 IffStream::getMatrix3x4Transform() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->readMatrix3x4Transform();
}

Vector3 IffStream::getVector3() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->readVector3();
}

Vector4 IffStream::getVector4() {
	if (openedChunk == NULL)
		throw NoOpenedChunkException(this);

	return openedChunk->readVector4();
}
