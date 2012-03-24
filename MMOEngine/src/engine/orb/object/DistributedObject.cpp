/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/orb/messages/DOBServiceClient.h"

#include "DistributedObject.h"

DistributedObject::DistributedObject() : Object() {
	_objectID = 0;

	_objectBroker = NULL;

	_updated = true;
	_markedForDeletion = false;
}

DistributedObject::~DistributedObject() {
}
