#include "Stream.h"

Stream::Stream() : Vector<char>() {
	end = offset = elementData;
}

Stream::Stream(int initsize) : Vector<char>(initsize) {
	end = offset = elementData;
}

Stream::Stream(int initsize, int capincr) : Vector<char>(initsize, capincr) {
	end = offset = elementData;
}

Stream::Stream(char *buf, int len) : Vector<char>(len) {
	offset = elementData;
	end = elementData + len;

	memcpy(elementData, buf, len);
}

Stream::~Stream() {
	end = offset = NULL;
}

Stream* Stream::clone(int startoffs) {
	int newSize = size() - startoffs;
	Stream* stream = new Stream(newSize);

	stream->writeStream(elementData + startoffs, newSize);

	return stream;		
}

void Stream::copy(Stream* stream, int startoffs) {
	int newSize = size() - startoffs;

	stream->reset();
	stream->writeStream(elementData + startoffs, newSize);
}

void Stream::setSize(int len, bool copyContent) {
	char* oldElementData = elementData;
	Vector<char>::setSize(len, copyContent);
	
	if (oldElementData != elementData)
		offset = (offset - oldElementData) + elementData;
	
	end = elementData + len;
}

void Stream::extendSize(int len, bool copyContent) {
	if ((offset += len) > end) {
		char* oldElementData = elementData;
		Vector<char>::setSize(offset - elementData);

		if (oldElementData != elementData)
			offset = (offset - oldElementData) + elementData;

		end = offset;
	}
}

void Stream::setOffset(int offs) {
	if ((offset = elementData + offs) > end)
		throw StreamIndexOutOfBoundsException(this, offs);
}

void Stream::shiftOffset(int offs) {
	if ((offset += offs) > end)
		throw StreamIndexOutOfBoundsException(this, offset - elementData);
}

void Stream::clear() {
	Vector<char>::setSize(0);
	 
	end = offset = elementData;
}

void Stream::reset() {
	offset = elementData;
}

void Stream::removeLastBytes(int len) {
	int newSize = size() - len;
	if (newSize < 0)
		throw StreamIndexOutOfBoundsException(this, newSize);

	setSize(newSize);
}

// stream manipulation methods
void Stream::writeStream(const char *buf, int len) {
	extendSize(len);

	memcpy(offset - len, buf, len);
}

void Stream::writeStream(Stream* stream) {
	writeStream(stream->getBuffer(), stream->size());
}

void Stream::writeStream(Stream* stream, int len) {
	if (len > stream->size())
		throw StreamIndexOutOfBoundsException(stream, len);

	writeStream(stream->getBuffer(), len);
}

void Stream::readStream(char *buf, int len) {
	shiftOffset(len);

	memcpy(buf, offset - len, len);
}

void Stream::readStream(Stream* stream, int len) {
	stream->setSize(len);

	readStream(stream->getBuffer(), stream->size());
}
