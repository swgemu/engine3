/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TransactionalObjectHeader.h"

#include "TransactionalObjectHandle.h"

using namespace engine::stm;

TransactionalObjectHandle::TransactionalObjectHandle(TransactionalObjectHeader* header) {
	TransactionalObjectHandle::header = header;

	object = header->getObject();

	objectCopy = (TransactionalObject*) malloc(sizeof(TransactionalObject));
	memcpy(objectCopy, object, sizeof(object));
}

TransactionalObjectHandle::~TransactionalObjectHandle() {
	header = NULL;
	object = NULL;

	free(objectCopy);
	objectCopy = NULL;
}

bool TransactionalObjectHandle::hasObjectChanged() {
	return memcmp(object, objectCopy, sizeof(object)) != 0 && object == header->getObject();
}
