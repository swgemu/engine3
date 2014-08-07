/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/orb/messages/RemoteObjectBroker.h"

#include "ObjectBrokerAgent.h"

#include "ControlMessage.h"

#include "ObjectBrokerDirector.h"

ObjectBrokerDirector::ObjectBrokerDirector() : Logger("ObjectBrokerDirector") {
	setDebugLogLevel();
}

ObjectBrokerDirector::~ObjectBrokerDirector() {
}

void ObjectBrokerDirector::start() {
	info("running as active director");
}

void ObjectBrokerDirector::createBackup() {
	sendCommand(CREATE_BACKUP);
}

void ObjectBrokerDirector::doStateUpdate(int state) {
	switch (state) {
	case ObjectBrokerAgent::STARTED:
		debug("agents started with " + String::valueOf(objectBrokerTable.getBrokerCount()) + " nodes");
		break;
	case ObjectBrokerAgent::BACKUP_STARTED:
		debug("backup started with " + String::valueOf(objectBrokerTable.getBrokerCount()) + " nodes");
		break;
	case ObjectBrokerAgent::IDLE:
		debug("node is IDLE");
		break;
	}
}

void ObjectBrokerDirector::handleStateUpdate(ObjectBroker* broker, int state) {
	Locker locker(this);

	debug("received state update " + String(ObjectBrokerAgent::stateToString((int) state)) + " from agent");

	if (agentStates.get(broker) == state) {
		debug("agent is already performing this action");

		return;
	}

	assert(!(agentStates.contains(broker) && agentStates.get(broker) == state));

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

	debug("sending command " + String(commandToString((int) command)) + " to agents");

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

	objectBrokerTable.remove(broker);
}


const char* ObjectBrokerDirector::commandToString(int command) {
	switch (command) {
	case CREATE_BACKUP:
		return "CREATE BACKUP";
	default:
		return "";
	}
}
