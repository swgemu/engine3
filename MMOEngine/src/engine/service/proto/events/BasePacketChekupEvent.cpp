/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "BasePacketChekupEvent.h"

BasePacketChekupEvent::BasePacketChekupEvent(BaseClient* cl, uint32 time) : Event(time) {
	client = cl;
	checkupTime = time;
			
	setKeeping(true);
}

bool BasePacketChekupEvent::activate() {
	client->checkupServerPackets(packet);

	return false;
}
