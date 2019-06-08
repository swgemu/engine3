/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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
