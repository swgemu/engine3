/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/


#include "BasePacketChekupEvent.h"

BasePacketChekupEvent::BasePacketChekupEvent(BaseClient* cl, uint32 time) : Task(time) {
	client = cl;
	checkupTime = time;

#if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
	setStatsSample(0);
#endif
}

void BasePacketChekupEvent::run() {
	Reference<BaseClient*> client = this->client.get();

	if (client != nullptr)
		client->checkupServerPackets(packet);
}
