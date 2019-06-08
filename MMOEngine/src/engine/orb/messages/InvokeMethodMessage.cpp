/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 14/10/16.
//

#include "InvokeMethodMessage.h"

#include "engine/orb/DistributedObjectBroker.h"


InvokeMethodMessage::InvokeMethodMessage(uint64 objectid, sys::uint32 methid, sys::uint32 invid, bool async) : DOBMessage(INVOKEMETHODMESSAGE, 40) {
	insertLong(objectid);

	insertInt(methid);
	insertInt(invid);

	insertBoolean(async);

	packet = nullptr;
	objectID = 0;
	methodID = 0;
	invocationID = 0;

	this->async = async;
}

InvokeMethodMessage::InvokeMethodMessage(Packet* message) : DOBMessage(message) {
	objectID = message->parseLong();

	methodID = message->parseInt();
	invocationID = message->parseInt();

	async = message->parseBoolean();

	packet = message->clone();
}

InvokeMethodMessage::~InvokeMethodMessage() {
	if (packet != nullptr)
		delete packet;
}

void InvokeMethodMessage::runMethod() {
	DistributedObjectBroker* orb = DistributedObjectBroker::instance();

	//printf("trying to get adapter in invoke method fod oid:%d", (int) objectID);

	DistributedObjectAdapter* adapter = orb->getObjectAdapter(objectID);
	if (adapter == nullptr) {
		orb->error("object not found for method invocation for oid: " + String::valueOf(objectID));
		return;
	}

	DistributedMethod invocation(orb, this);
	adapter->invokeMethod(methodID, &invocation);

	if (!async) {
		DOBMessage* response = (DOBMessage*) invocation.getResponseMessage();
		client->sendReply(response);
	}
}

void InvokeMethodMessage::execute() {
	if (async) {
		class RunTask : public Task {
		protected:
			InvokeMethodMessage* method;

		public:
			RunTask(InvokeMethodMessage* method) : method(method) {

			}

			void run() {
				method->runMethod();
			}
		};

		RunTask* task = new RunTask(this);
		task->execute();
	} else {
		runMethod();
	}

}
