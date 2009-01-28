/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "BaseFragmentedPacket.h"

BaseFragmentedPacket::BaseFragmentedPacket() : BasePacket() {
	singlePacket = NULL;

	offset = 0;
}

BaseFragmentedPacket::BaseFragmentedPacket(BasePacket* pack) : BasePacket() {
	singlePacket = pack;

	offset = 0;
}

BaseFragmentedPacket::~BaseFragmentedPacket() {
	if (singlePacket != NULL)
		delete singlePacket;
}

void BaseFragmentedPacket::addFragment(Packet* pack) {
	uint16 seq = pack->parseNetShort(2);

	if (!hasData()) {
		sequence = seq;

		offset = pack->parseNetInt(4);

		insertStream(pack->getBuffer() + 8, 496 - 8 - 3);
	} else {
		if (sequence != seq)
			throw Exception("fragmented packet sequence does not match");

		int fragsize = MIN(496, pack->size()) - 4 - 3;

		insertStream(pack->getBuffer() + 4, fragsize);
	}
}

BasePacket* BaseFragmentedPacket::getFragment() {
	BasePacket* frag = new BasePacket();
	frag->setCompression(true);

	frag->insertShort(0x0D00);
	frag->insertShort(0x0000);

	int fragsize;

	if (offset == 0) {
		frag->insertIntNet(singlePacket->size() - 4);

		offset = 4;
		fragsize = 496 - 8 - 3;
	} else {
		fragsize = MIN(496 - 4 - 3, singlePacket->size() - offset);
	}

	frag->insertStream(singlePacket->getBuffer() + offset, fragsize);
	offset += fragsize;

	//System::out << "fragged packet: " << frag->toString() << "\n";

	return frag;
}

bool BaseFragmentedPacket::isComplete() {
	int currentSize = size();

	if (currentSize < offset)
		return false;
	else if (currentSize == offset)
		return true;
	else
		throw Exception("fragmented packet exceeded size");
}

bool BaseFragmentedPacket::hasFragments() {
	return offset < singlePacket->size();
}
