/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "BaseFragmentedPacket.h"

BaseFragmentedPacket::BaseFragmentedPacket(BasePacket* pack) : BasePacket() {
	singlePacket = pack;
	
	offset = 0;
}

BaseFragmentedPacket::~BaseFragmentedPacket() {
	delete singlePacket;
}

BasePacket* BaseFragmentedPacket::nextPacket() {
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

bool BaseFragmentedPacket::hasNext() {
	return offset < singlePacket->size();
}
