/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "Packet.h"

Packet::Packet() : Stream(RAW_MAX_SIZE, RAW_MAX_SIZE), ObjectInputStream(), ObjectOutputStream() {
}

Packet::Packet(int size) : Stream(size, RAW_MAX_SIZE), ObjectInputStream(), ObjectOutputStream() {
}

Packet::~Packet() {
}

/*void Packet::create(char *buf[], int len) {
	setSize(len);

	memcpy(elementData, buf, len);
}*/

Packet* Packet::clone(int startoffs) {
	int newSize = size() - startoffs;
	Packet* p = new Packet(newSize);
	p->insertStream(elementData + startoffs, newSize);

	return p;
}

/*void Packet::copy(Packet* pack, int startoffs) {
	int newSize = size() - startoffs;

	pack->reset();
	pack->insertStream(elementData + startoffs, newSize);
}

void Packet::setSize(int len, bool copyContent) {
	char* oldElementData = elementData;
	Vector<char>::setSize(len, copyContent);

	if (oldElementData != elementData)
		offset = (offset - oldElementData) + elementData;

	end = elementData + len;
}

void Packet::extendSize(int len, bool copyContent) {
	if ((offset += len) > end) {
		char* oldElementData = elementData;
		Vector<char>::setSize(offset - elementData);

		if (oldElementData != elementData)
			offset = (offset - oldElementData) + elementData;

		end = offset;
	}
}

void Packet::clear() {
	Vector<char>::setSize(0);

	end = offset = elementData;
}

void Packet::reset() {
	offset = elementData;
}

void Packet::removeLastBytes(int len) {
	int newSize = size() - len;
	if (newSize < 0)
		throw PacketIndexOutOfBoundsException(this, newSize);

	setSize(newSize);
}*/

String Packet::toStringData() {
	StringBuffer str;
	str << "Packet [" << size() << "] " << uppercase << hex;

	for (int i = 0; i < size(); ++i) {
		unsigned int byte = ((unsigned int) elementData[i]) & 0xFF;

		if ((byte & 0xF0) == 0)
			str << "0" << hex << byte  << " ";
		else
			str << hex << byte  << " ";
	}

	return str.toString();
}
