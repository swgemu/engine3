/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "DistributedObjectBroker.h"

#include "packets/DeployObjectMessage.h"
#include "packets/LookUpObjectMessage.h"
#include "packets/LookUpObjectResponseMessage.h"

#include "DOBPacketHandler.h"

DOBPacketHandler::DOBPacketHandler() : Logger() {
	orb = NULL;
} 

DOBPacketHandler::DOBPacketHandler(const String& s, DistributedObjectBroker* broker) : Logger(s) {
	orb = broker;
} 

DOBPacketHandler::~DOBPacketHandler() {
}
	
void DOBPacketHandler::handlePacket(DistributedObjectBrokerClient* client, Packet* pack) {
	//System::out << "READ " << pack->toString() << "\n";

	uint32 opcode = pack->parseInt();

	switch (opcode) {
	case 01:
		handleObjectLookUpMessage(client, pack);
		break;
	case 03:
		handleObjectDeployMessage(client, pack);
		break;
	case 05:
		handleMethodInvocationMessage(client, pack);
		break;
	}
}

void DOBPacketHandler::handleObjectLookUpMessage(DistributedObjectBrokerClient* client, Packet* pack) {
	String name;
	LookUpObjectMessage::parseObjectName(pack, name);

	DistributedObject* obj = orb->lookUp(name);

	Packet* msg = new LookUpObjectResponseMessage(obj);
	client->send(msg);
}

void DOBPacketHandler::handleObjectDeployMessage(DistributedObjectBrokerClient* client, Packet* pack) {
	String name;
	pack->parseAscii(name);

	String classname;
	pack->parseAscii(classname);

	uint64 objid = 0;

	/*ORBClassHelper* helper = classMap.get(classname);
	if (helper != NULL) {
		
		
		orb->deploy()
	}
	
	Packet* msg = new DeployObjectResponseMessage(objid);
	client->send(msg);*/
}

void DOBPacketHandler::handleMethodInvocationMessage(DistributedObjectBrokerClient* client, Packet* pack) {
	uint64 objectid = pack->parseLong();

	DistributedObjectAdapter* adapter = orb->getObjectAdapter(objectid);
	if (adapter == NULL) {
		error("object not found for method invocation");
		return;
	}
	
	uint32 methodID = pack->parseInt();
	uint32 invocationID = pack->parseInt();

	DistributedMethod invocation(orb, pack);

	Packet* resp = adapter->invokeMethod(methodID, &invocation);
	client->send(resp);
}
