/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "RUDPPacket.h"

RUDPPacket::RUDPPacket() : Message() {
	doSeq = true;

	doEncr = true;
	doComp = false;
	doCRCTest = true;
}

RUDPPacket::RUDPPacket(int size) : Message(size) {
	if (size < 0)
		throw PacketIndexOutOfBoundsException(this, size);
	
	doSeq = true;

	doEncr = true;
	doComp = false;
	doCRCTest = true;
}

RUDPPacket::RUDPPacket(Packet* pack, int seq) : Message(pack->size()) {
	pack->copy(this);
	
	setOffset(pack->getOffset());

	doSeq = true;
	sequence = seq;
}

RUDPPacket* RUDPPacket::clone(int startoffs) {
	RUDPPacket* pack = new RUDPPacket(elementCount - startoffs);
	copy(pack, startoffs);
	
	pack->doSeq = doSeq;
	pack->doEncr = doEncr;
	pack->doComp = doComp;
	pack->doCRCTest = doCRCTest;
	
	return pack;
}

int RUDPPacket::compareTo(RUDPPacket* pack) {
	if (sequence < pack->sequence)
		return 1;
	else if (sequence > pack->sequence)
		return -1;
	else
		return 0; 
}

void RUDPPacket::close() {
	if (size() < 80)
		insertByte(doComp = false);
	else
		insertByte(doComp);
		
	insertByte(0x00);
	insertByte(0x00);
}
