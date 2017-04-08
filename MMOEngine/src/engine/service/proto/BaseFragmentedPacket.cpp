/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "BaseFragmentedPacket.h"

#include "engine/log/Logger.h"

BaseFragmentedPacket::BaseFragmentedPacket() : BasePacket() {
	singlePacket = NULL;

	totalSize = 0;
}

BaseFragmentedPacket::BaseFragmentedPacket(BasePacket* pack) : BasePacket() {
	singlePacket = pack;

	totalSize = 0;
}

BaseFragmentedPacket::~BaseFragmentedPacket() {
	if (singlePacket != NULL)
		delete singlePacket;
}

bool BaseFragmentedPacket::addFragment(Packet* pack) {
	//uint32 seq = pack->parseNetShort();

	if (totalSize == 0) {
		totalSize = pack->parseNetInt();

		if (totalSize < 0 || totalSize > 50000) {
			Logger::console.error("received fragmented packet with size too big = (" + String::valueOf(totalSize) + ")");
			return false;
		} else if (totalSize == 0) {
			Logger::console.error("fragmented total size totalSize parsed 0!");
		}

		/*Logger::console.info("received first segment of fragmented packet ("
						+ String::valueOf(seq) + ") - size = " + String::valueOf(totalSize));*/
	}

	int packetOffset = pack->getOffset();

	int readBytes = pack->size() - packetOffset;

	if (readBytes + size() > totalSize)
		readBytes = totalSize - size();

	if (readBytes < 0) {
		Logger::console.error("error parsing fragmented packet readBytes < 0");
		return false;
	} /*else {
		Logger::console.info("readBytes = " + String::valueOf(readBytes), true);
	}*/

	insertStream(pack->getBuffer() + packetOffset, readBytes/* - 3*/);

	/*Logger::console.info("received next segment of fragmented packet ("
				+ String::valueOf(seq) + ") - size = " + String::valueOf(totalSize));*/

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

	/*Logger::console.info("checking fragmented packet completeness: " + String::valueOf(currentSize)
			+ " = " + String::valueOf(totalSize));*/

	if (currentSize < totalSize)
		return false;
	else if (currentSize == totalSize) {
		//Logger::console.info("currentSize == totalSize : " + String::valueOf(totalSize), true);

		return true;
	} else
		throw Exception("fragmented packet exceeded size (" + String::valueOf(totalSize)
				+ ") - size = " + String::valueOf(currentSize));
}

bool BaseFragmentedPacket::hasFragments() {
	return totalSize < singlePacket->size();
}
