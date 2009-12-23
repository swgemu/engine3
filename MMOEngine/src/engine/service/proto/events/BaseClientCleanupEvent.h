#ifndef BASECLIENTCLEANUPEVENT_H_
#define BASECLIENTCLEANUPEVENT_H_

#include "system/lang.h"

#include "../BaseClient.h"

class BaseClientCleanupEvent : public Task {
	Reference<BaseClient*> client;
	
public:
	BaseClientCleanupEvent(BaseClient* cl) : Task(60000) {
		client = cl;
	}
	
	void run() {
		client = NULL;
	}
	
};

#endif /*BASECLIENTCLEANUPEVENT_H_*/
