/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "Stream.h"

Stream::Stream() : ArrayList<char>() {
	end = offset = elementData;
}

Stream::Stream(int initsize) : ArrayList<char>(initsize, initsize / 2) {
	end = offset = elementData;
}

Stream::Stream(int initsize, int capincr) : ArrayList<char>(initsize, capincr) {
	end = offset = elementData;
}

Stream::Stream(const char *buf, int len) : ArrayList<char>(len, len / 2) {
	end = offset = elementData;

	writeStream(buf, len);

	reset();
}

Stream::~Stream() {
	end = offset = nullptr;
}

Stream* Stream::clone(int startoffs) {
	if (startoffs) {
		E3_ASSERT(startoffs < size());
	}

	int newSize = size() - startoffs;
	Stream* stream = new Stream(newSize);

	stream->writeStream(elementData + startoffs, newSize);

	return stream;
}

void Stream::copy(Stream* stream, int startoffs) const {
	if (startoffs) {
		E3_ASSERT(startoffs < size());
	}

	int newSize = size() - startoffs;

	stream->reset();

	if (elementData) {
		stream->writeStream(elementData + startoffs, newSize);
	}

	/* TA: resetting the offset breaks message->clone() for BaseProtocol
	 * because it expects the offset to be at the end.
	 * if startoffs == 0, means we copy all the packet -> we can copy the offset too of the original packet
	 */

	if (startoffs == 0)
		stream->offset = stream->elementData + (offset - elementData);
}

void Stream::setSize(int len, bool copyContent) {
	char* oldElementData = elementData;
	ArrayList<char>::setSize(len, copyContent);

	if (oldElementData != elementData)
		offset = (offset - oldElementData) + elementData;

	end = elementData + len;
}

void Stream::extendSize(int len, bool copyContent) {
	if ((offset += len) > end) {
		char* oldElementData = elementData;
		ArrayList<char>::setSize(offset - elementData);

		if (oldElementData != elementData)
			offset = (offset - oldElementData) + elementData;

		end = offset;

		E3_ASSERT(offset);
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
	ArrayList<char>::setSize(0);

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

void Stream::removeRange(int fromIndex, int toIndex) {
	char* oldElementData = elementData;

	ArrayList<char>::removeRange(fromIndex, toIndex);

	end = elementData + ArrayList<char>::size();

	if (oldElementData != elementData || offset > end)
		offset = (offset - oldElementData) + elementData;
}

void Stream::removeAll(int newSize, int newIncrement) {
	removeRange(0, size());
}

// stream manipulation methods
void Stream::writeStream(const char *buf, int len) {
	if (len) {
		E3_ASSERT(buf);

		extendSize(len);

		memcpy(offset - len, buf, len);
	}
}

void Stream::writeStream(const Stream* stream) {
	writeStream(stream->getBuffer(), stream->size());
}

void Stream::writeStream(const Stream* stream, int len) {
	if (len > stream->size())
		throw StreamIndexOutOfBoundsException(stream, len);

	writeStream(stream->getBuffer(), len);
}

void Stream::writeStream(const Stream* stream, int len, int offs) {
	if (len > stream->size())
		throw StreamIndexOutOfBoundsException(stream, len);

	if (stream->getBuffer() == nullptr) {
		return;
	}

	if ((elementData + offs + len) > end) {
		setSize(offs + len);
	}

	memcpy(elementData + offs, stream->getBuffer(), len);
}

void Stream::insertStream(const Stream* stream, int len, int offs) {
	if (len > stream->size())
		throw StreamIndexOutOfBoundsException(stream, len);

	if (stream->getBuffer() == nullptr) {
		return;
	}

	if (elementData + offs > end) {
		writeStream(stream, len);

		return;
	}

	char* oldElementData = elementData;

	for (int i = 0; i < stream->size(); ++i) {
		ArrayList<char>::insertElementAt(stream->getBuffer()[i], offs + i);
	}

	if (oldElementData != elementData)
		offset = (offset - oldElementData) + elementData;

	end = elementData + ArrayList<char>::size();
}

void Stream::readStream(char *buf, int len) {
	if (len) {
		E3_ASSERT(buf);

		shiftOffset(len);

		memcpy(buf, offset - len, len);
	}
}

void Stream::readStream(Stream* stream, int len) {
	stream->setSize(len);

	readStream(stream->getBuffer(), stream->size());
}

String Stream::toStringData() const {
	StringBuffer str;
	str << "Stream [" << size() << "] " << uppercase << hex;

	for (int i = 0; i < size(); ++i) {
		unsigned int byte = ((unsigned int) elementData[i]) & 0xFF;

		if ((byte & 0xF0) == 0)
			str << "0" << byte  << " ";
		else
			str << byte  << " ";
	}

	return str.toString();
}

