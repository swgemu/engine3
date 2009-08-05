#ifndef BASECLIENTCLEANUPEVENT_H_
#define BASECLIENTCLEANUPEVENT_H_

#include "system/platform.h"

#include "../BaseClient.h"

class BaseClientCleanupEvent : public Task {
	BaseClient* client;
	
public:
	BaseClientCleanupEvent(BaseClient* cl) : Task(60000) {
		client = cl;
	}
	
	void run() {
		client->finalize();
		client = NULL;
	}
	
};

#endif /*BASECLIENTCLEANUPEVENT_H_*/
