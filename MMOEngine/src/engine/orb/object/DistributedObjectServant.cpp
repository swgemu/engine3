/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../DistributedObjectBroker.h"

#include "DistributedObjectStub.h"
#include "DistributedObjectServant.h"

DistributedObjectServant::DistributedObjectServant() {
}

DistributedObjectServant::~DistributedObjectServant() {
}

void DistributedObjectServant::setDeployingName(const String& name) {
	_stub.get()->_setName(name);
}

String DistributedObjectServant::toString() {
	return _stub.get()->_getName();
}
