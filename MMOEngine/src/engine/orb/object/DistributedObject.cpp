/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../DistributedObjectBrokerClient.h"

#include "DistributedObject.h"

DistributedObject::DistributedObject() {
	_objectID = 0;
}

DistributedObject::~DistributedObject() {
	printf("deleting DistributedObject\n");
}
