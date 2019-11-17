/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/messages/RemoteObjectBroker.h"

#include "ObjectBrokerAgent.h"

#include "ControlMessage.h"

ObjectBrokerDirector::ObjectBrokerDirector() : Logger("ObjectBrokerDirector") {
	setInfoLogLevel();
}

ObjectBrokerDirector::~ObjectBrokerDirector() {
}

void ObjectBrokerDirector::start() {
	info("running as active director");
}

void ObjectBrokerDirector::createBackup(bool full) {
	if (full) {
		sendCommand(CREATE_FULL_BACKUP);
	} else {
		sendCommand(CREATE_BACKUP);
	}
}

void ObjectBrokerDirector::doStateUpdate(int state) {
	switch (state) {
	case ObjectBrokerAgent::STARTED:
		debug() << "agents started with " << objectBrokerTable.getBrokerCount() << " nodes";
		break;
	case ObjectBrokerAgent::BACKUP_STARTED:
		debug() << "backup started with " << objectBrokerTable.getBrokerCount() << " nodes";
		break;
	case ObjectBrokerAgent::IDLE:
		debug("node is IDLE");
		break;
	}
}

void ObjectBrokerDirector::handleStateUpdate(ObjectBroker* broker, int state) {
	Locker locker(this);

	debug() << "received state update " << ObjectBrokerAgent::stateToString((int) state) << " from agent";

	if (agentStates.get(broker) == state) {
		debug("agent is already performing this action");

		return;
	}

	fatal(!(agentStates.contains(broker) && agentStates.get(broker) == state)) << "agent states doesnt contain broker";

	int minState = state;
	int maxState = 0;

	for (int i = 0; i < agentStates.size(); ++i) {
		int agentState = agentStates.get(i);

		if (agentState < minState)
			minState = agentState;

		if (agentState > maxState)
			maxState = agentState;
	}

	agentStates.put(broker, state);

	if (agentStates.size() == objectBrokerTable.getBrokerCount() && minState == maxState) {
		agentStates.removeAll();

		doStateUpdate(minState);
	}
}

void ObjectBrokerDirector::sendCommand(Command command) {
	Locker locker(this);

	debug() << "sending command " << commandToString((int) command) << " to agents";

	ControlMessage controlMessage((int) command);

	HashSetIterator<ObjectBroker*> iter = objectBrokerTable.iterator();
	while (iter.hasNext()) {
		ObjectBroker* broker = iter.next();
		if (broker != DistributedObjectBroker::instance()) {
			DOBServiceClient* agentPort = static_cast<RemoteObjectBroker*>(broker)->getBrokerClient();
			agentPort->send(&controlMessage);
		} else {
			ObjectBrokerAgent::instance()->doCommand(command);
		}
	}
}

void ObjectBrokerDirector::brokerConnected(ObjectBroker* broker) {
	Locker locker(this);

	objectBrokerTable.add(broker);
}

void ObjectBrokerDirector::brokerDisconnected(ObjectBroker* broker) {
	Locker locker(this);

	info(true) << "broker disconnected";

	RemoteObjectBroker* remote = dynamic_cast<RemoteObjectBroker*>(broker);

	if (remote) { // this assumes no other deploy messaage will arrive while this is running
		SynchronizedSortedVector<DistributedObject*>& deployedObjects = remote->getDeployedObjects();

		for (int i = 0; i < deployedObjects.size(); ++i) {
			DistributedObject* obj = deployedObjects.get(i);

			if (obj->_getObjectBroker() == remote)
				obj->_setObjectBroker(nullptr);
		}

		deployedObjects.removeAll();
	}

	objectBrokerTable.remove(broker);

	agentStates.drop(broker);
}

const char* ObjectBrokerDirector::commandToString(int command) {
	switch (command) {
	case CREATE_BACKUP:
		return "CREATE BACKUP";
	default:
		return "";
	}
}
