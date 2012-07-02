/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "BasePacket.h"

BasePacket::BasePacket() : Message() {
	doSeq = true;

	doEncr = true;
	doComp = false;
	doCRCTest = true;
	
	outOfOrderCount = 0;
}

BasePacket::BasePacket(int size) : Message(size) {
	if (size < 0)
		throw PacketIndexOutOfBoundsException(this, size);
	
	doSeq = true;

	doEncr = true;
	doComp = false;
	doCRCTest = true;
	
	outOfOrderCount = 0;
}

BasePacket::BasePacket(Packet* pack, int seq) : Message(pack->size()) {
	pack->copy(this);
	
	setOffset(pack->getOffset());

	doSeq = true;
	sequence = seq;
	
	outOfOrderCount = 0;
}

BasePacket* BasePacket::clone(int startoffs) {
	BasePacket* pack = new BasePacket(elementCount - startoffs);
	copy(pack, startoffs);
	
	pack->doSeq = doSeq;
	pack->doEncr = doEncr;
	pack->doComp = doComp;
	pack->doCRCTest = doCRCTest;
	
	return pack;
}

int BasePacket::compareTo(BasePacket* pack) {
	if (sequence < pack->sequence)
		return 1;
	else if (sequence > pack->sequence)
		return -1;
	else
		return 0; 
}

void BasePacket::close() {
	if (size() < 100)
		insertByte(doComp = false);
	else
		insertByte(doComp);
		
	insertByte(0x00);
	insertByte(0x00);
}
