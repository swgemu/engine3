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

Packet* Packet::clone(int startoffs) {
	Packet* pack = new Packet();
	copy(pack, startoffs);

	return pack;
}

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
