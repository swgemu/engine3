/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "../DistributedObjectBrokerClient.h"

#include "DistributedObject.h"

DistributedObject::DistributedObject() {
}

DistributedObject::~DistributedObject() {
}

void DistributedObject::finalize() {
	Object::finalize();
}

void DistributedObject::acquire() {
	Object::acquire();

	/*StringBuffer msg;
	msg << "[" << _getName() << "] acquired (" << getReferenceCount() << ")";
	Logger::console.info(msg);*/
}

void DistributedObject::release() {

	/*StringBuffer msg;
	msg << "[" << _getName() << "] released (" << getReferenceCount() - 1 << ")";
	Logger::console.info(msg);*/

	Object::release();
}
