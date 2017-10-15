/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/


#include "BasePacketChekupEvent.h"

BasePacketChekupEvent::BasePacketChekupEvent(BaseClient* cl, uint32 time) : Task(time) {
	client = cl;
	checkupTime = time;

#ifdef BASECLIENT_DISABLE_STATSD
	setStatsSample(0);
#endif
}

void BasePacketChekupEvent::run() {
	Reference<BaseClient*> client = this->client.get();

	if (client != NULL)
		client->checkupServerPackets(packet);
}
