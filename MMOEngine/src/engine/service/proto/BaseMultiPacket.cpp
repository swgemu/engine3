/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "BaseMultiPacket.h"

BaseMultiPacket::BaseMultiPacket(BasePacket* pack) : BasePacket() {
	singlePacket = pack;

	setSequencing(true);
}

BaseMultiPacket::~BaseMultiPacket() {
	//if (singlePacket != nullptr)
	//	delete singlePacket;
}
	
bool BaseMultiPacket::add(BasePacket* pack) {
	/*if (pack->size() - 4 > 0xFF);
		return false;*/

	if (singlePacket != nullptr) {
		insertShort(0x0900);
		insertShort(0);
		insertShort(0x1900);
		
		insertPacket(singlePacket);
		
		setCompression(singlePacket->doCompression());
		
		delete singlePacket;
		singlePacket = nullptr;
	}
	
	if (size() + pack->size() > 460)
		return false;
		
	insertPacket(pack);

	setCompression(doCompression() || pack->doCompression());

	delete pack;
	return true;
}

void BaseMultiPacket::insertPacket(BasePacket* pack) {
	int size = pack->size() - 4;
		
	if (size >= 0xFF) {
		insertByte(0xFF);
		insertShortNet(size);

		//something wrong here client is sending out of orders for our multi packets with size >= 0xFF
		/*if (size == 0xFF) {
			//insertShortNet(size - 0xFF);
			insertByte(0);
			insertByte(0xFF);
		} else {
			insertByte(1);
			insertByte(size - 0xFF);
		}*/

	} else
		insertByte(size);
		
	insertStream(pack->getBuffer() + 4, size);
}

BasePacket* BaseMultiPacket::getPacket() {
	if (singlePacket != nullptr) {
		BasePacket* pack = singlePacket;
		
		delete this;
		return pack;
	} else
		return this;
}
