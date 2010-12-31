/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/db/ObjectDatabaseManager.h"

#include "TransactionalObjectManager.h"

TransactionalObjectManager::TransactionalObjectManager() : Command() {
}

void TransactionalObjectManager::initialize() {
	ObjectDatabaseManager::instance()->startLocalTransaction();
}

void TransactionalObjectManager::execute() {
	ObjectDatabaseManager::instance()->commitLocalTransaction();
}

void TransactionalObjectManager::undo() {
	ObjectDatabaseManager::instance()->abortLocalTransaction();
}
