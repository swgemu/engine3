/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef BASECLIENTCLEANUPEVENT_H_
#define BASECLIENTCLEANUPEVENT_H_

#include "system/lang.h"

#include "engine/service/proto/BaseClient.h"

class BaseClientCleanupEvent : public Task {
	Reference<BaseClient*> client;

public:
	BaseClientCleanupEvent(BaseClient* cl) : Task(60000) {
		client = cl;

#if defined(BASECLIENT_DISABLE_STATSD) and defined(COLLECT_TASKSTATISTICS)
		setStatsSample(0);
#endif
	}

	void run() {
		client = nullptr;
	}

};

#endif /*BASECLIENTCLEANUPEVENT_H_*/
