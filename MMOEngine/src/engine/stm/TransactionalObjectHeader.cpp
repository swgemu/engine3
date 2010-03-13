/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "TransactionalObjectHeader.h"

using namespace engine::stm;

TransactionalObject* TransactionalObjectHeader::get() {
	return Transaction::currentTransaction()->openObject(this);
}

TransactionalObject* TransactionalObjectHeader::getForUpdate() {
	return Transaction::currentTransaction()->openObjectForWrite(this);
}

bool TransactionalObjectHeader::acquire(Transaction* transaction) {
	return Atomic::compareAndSwap((uint32*) ownerTransaction, 0, (uint32) transaction);
}

void TransactionalObjectHeader::release(TransactionalObject* newobject) {
	object = newobject;

	ownerTransaction = NULL;
}
