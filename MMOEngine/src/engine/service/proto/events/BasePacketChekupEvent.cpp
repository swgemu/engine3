/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/


#include "BasePacketChekupEvent.h"

BasePacketChekupEvent::BasePacketChekupEvent(BaseClient* cl, uint32 time) : Task(time) {
	client = cl;
	checkupTime = time;
	sequence = 0;
}

void BasePacketChekupEvent::run() {
	client->checkupServerPackets(sequence);
}
