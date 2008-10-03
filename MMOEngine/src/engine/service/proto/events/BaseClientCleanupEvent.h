#ifndef BASECLIENTCLEANUPEVENT_H_
#define BASECLIENTCLEANUPEVENT_H_

#include "../../../../system/platform.h"

#include "../BaseClient.h"

class BaseClientCleanupEvent : public Event {
	BaseClient* client;
	
public:
	BaseClientCleanupEvent(BaseClient* cl) : Event(60000) {
		client = cl;
	}
	
	bool activate() {
		client->finalize();
		client = NULL;
		
		return true;
	}
	
};

#endif /*BASECLIENTCLEANUPEVENT_H_*/
