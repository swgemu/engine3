/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "BaseFragmentedPacket.h"

#include "engine/log/Logger.h"

#define MAX_COMPLETE_FRAG_SIZE 1500000

namespace BPDetail {
	static Logger logger("BaseFragmentedPacket");
}

using namespace BPDetail;

BaseFragmentedPacket::BaseFragmentedPacket() : BasePacket() {
	singlePacket = nullptr;

	totalSize = 0;
}

BaseFragmentedPacket::BaseFragmentedPacket(BasePacket* pack) : BasePacket() {
	singlePacket = pack;

	totalSize = 0;
}

BaseFragmentedPacket::~BaseFragmentedPacket() {
	if (singlePacket != nullptr)
		delete singlePacket;
}

bool BaseFragmentedPacket::addFragment(Packet* pack) {
	if (totalSize == 0) {
		totalSize = pack->parseNetInt();

		if (totalSize < 0 || totalSize > MAX_COMPLETE_FRAG_SIZE) {
			addError() << "received fragmented packet with size too big = (" << totalSize << ") for frag: " << pack->toStringData();

			return false;
		} else if (totalSize == 0) {
			addError() << "fragmented total size totalSize parsed 0!";

			return false;
		}
	}

	int packetOffset = pack->getOffset();

	int readBytes = pack->size() - packetOffset;

	if (readBytes + size() > totalSize)
		readBytes = totalSize - size();

	if (readBytes < 0) {
		addError() << "error parsing fragmented packet readBytes < 0";
		return false;
	}

	insertStream(pack->getBuffer() + packetOffset, readBytes/* - 3*/);

	return true;
}

BasePacket* BaseFragmentedPacket::getFragment() {
	BasePacket* frag = new BasePacket();
	frag->setCompression(true);

	frag->insertShort(0x0D00);
	frag->insertShort(0x0000);

	int fragsize;

	if (totalSize == 0) {
		frag->insertIntNet(singlePacket->size() - 4);

		totalSize = 4;
		fragsize = 496 - 8 - 3;
	} else {
		fragsize = Math::min(496 - 4 - 3, singlePacket->size() - totalSize);
	}

	frag->insertStream(singlePacket->getBuffer() + totalSize, fragsize);
	totalSize += fragsize;

	//System::out << "fragged packet: " << frag->toString() << "\n";

	return frag;
}

bool BaseFragmentedPacket::isComplete() {
	int currentSize = size();

	if (currentSize < totalSize)
		return false;
	else if (currentSize == totalSize) {
		return true;
	} else {
		addError() << "fragmented packet exceeded size (" << totalSize << ") - size = " << currentSize;
		throw Exception(getError());
	}
}

bool BaseFragmentedPacket::hasFragments() const {
	return totalSize < singlePacket->size();
}
