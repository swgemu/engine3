/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/orb/messages/DOBServiceClient.h"

#include "DistributedObject.h"

DistributedObject::DistributedObject() : Object(), _objectID(0), _objectBroker(nullptr),
	_updated(true), _markedForDeletion(false), _deletedFromDatabase(false) {
}

DistributedObject::~DistributedObject() {
}
