/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/messages/RemoteObjectBroker.h"

#include "engine/orb/db/DOBObjectManager.h"

#include "StateUpdateMessage.h"

#include "ObjectBrokerAgent.h"

ObjectBrokerAgent::ObjectBrokerAgent() : Logger("ObjectBrokerAgent") {
	setInfoLogLevel();

	info("starting up agent");

	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	if (!broker->isRootBroker()) {
		RemoteObjectBroker* rootBroker = static_cast<RemoteObjectBroker*>(broker->getRootBroker());

		directorPort = static_cast<RemoteObjectBroker*>(rootBroker)->getBrokerClient();
	} else {
		directorPort = nullptr;
	}
}

ObjectBrokerAgent::~ObjectBrokerAgent() {
}

void ObjectBrokerAgent::start() {
	setState(STARTED);
}

void ObjectBrokerAgent::startBackup(int flags) {
	info("creating backup");

	setState(BACKUP_STARTED);

	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	if (broker->isRootBroker()) {
		DOBObjectManager* objectManager = broker->getObjectManager();
		fatal(objectManager) << "objectManager is null";

		objectManager->updateModifiedObjectsToDatabase(flags);
	} else {
		info("save events disabled on non root brokers", true);

		finishBackup();
	}
}

void ObjectBrokerAgent::finishBackup() {
	info("backup finished");

	setState(IDLE);
}

void ObjectBrokerAgent::doCommand(ObjectBrokerDirector::Command command, int flags) {
	debug() << "received command " <<
			ObjectBrokerDirector::commandToString((int) command, flags)
			<< " from director";

	if (command == ObjectBrokerDirector::CREATE_BACKUP) {
		startBackup(flags);
	} else {
		error() << "ObjectBrokerAgent::doCommand: Unknown command=" << command << ", flags=" << flags;
	}
}

void ObjectBrokerAgent::setState(AgentState state) {
	StateUpdateMessage stateUpdateMessage((int) state);

	debug() << "sending state update " << stateToString((int) state) << " to director";

	if (directorPort != nullptr) {
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
