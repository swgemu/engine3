/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/messages/RemoteObjectBroker.h"

#include "engine/orb/db/DOBObjectManager.h"

#include "StateUpdateMessage.h"

#include "ObjectBrokerAgent.h"

ObjectBrokerAgent::ObjectBrokerAgent() : Logger("ObjectBrokerAgent") {
	setDebugLogLevel();

	info("starting up agent");

	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	if (!broker->isRootBroker()) {
		RemoteObjectBroker* rootBroker = static_cast<RemoteObjectBroker*>(broker->getRootBroker());

		directorPort = static_cast<RemoteObjectBroker*>(rootBroker)->getBrokerClient();
	} else {
		directorPort = NULL;
	}
}

ObjectBrokerAgent::~ObjectBrokerAgent() {
}

void ObjectBrokerAgent::start() {
	setState(STARTED);
}

void ObjectBrokerAgent::startBackup() {
	info("creating backup");

	setState(BACKUP_STARTED);

	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	if (broker->isRootBroker()) {
		DOBObjectManager* objectManager = broker->getObjectManager();
		assert(objectManager);
		objectManager->updateModifiedObjectsToDatabase();
	} else {
		info("save events disabled on non root brokers", true);

		finishBackup();
	}
}

void ObjectBrokerAgent::finishBackup() {
	info("backup finished");

	setState(IDLE);
}

void ObjectBrokerAgent::doCommand(ObjectBrokerDirector::Command command) {
	debug("received command " + String(ObjectBrokerDirector::commandToString((int) command)) + " from director");

	switch (command) {
		case ObjectBrokerDirector::CREATE_BACKUP:
			startBackup();
			break;
	}
}

void ObjectBrokerAgent::setState(AgentState state) {
	StateUpdateMessage stateUpdateMessage((int) state);

	debug("sending state update " + String(stateToString((int) state)) + " to director");

	if (directorPort != NULL) {
		directorPort->send(&stateUpdateMessage);
	} else {
		ObjectBrokerDirector::instance()->handleStateUpdate(DistributedObjectBroker::instance(), (int) state);
	}
}

const char* ObjectBrokerAgent::stateToString(int state) {
	switch (state) {
	case STARTED:
		return "STARTED";
	case IDLE:
		return "IDLE";
	case BACKUP_STARTED:
		return "BACKUP STARTED";
	case BACKUP_FINISHED:
		return "BACKUP FINISHED";
	default:
		return "";
	}
}
