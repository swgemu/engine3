/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "MysqlDatabaseManager.h"

void MysqlDatabaseManager::commitModifiedDatabases() {
	auto localModifiedDatabases = getLocalModifiedDatabases();
	for (int i = 0; i < localModifiedDatabases->size(); ++i) {
		auto database = localModifiedDatabases->get(i);

		database->commit();
	}
}

void MysqlDatabaseManager::rollbackModifiedDatabases() {
	VectorSet<Database*>* localModifiedDatabases = getLocalModifiedDatabases();
	for (int i = 0; i < localModifiedDatabases->size(); ++i) {
		auto database = localModifiedDatabases->get(i);

		database->rollback();
	}
}

void MysqlDatabaseManager::addModifiedDatabase(Database* database) {
	auto localModifiedDatabases = getLocalModifiedDatabases();
	localModifiedDatabases->add(database);
}

VectorSet<Database*>* MysqlDatabaseManager::getLocalModifiedDatabases() {
	auto localModifiedDatabases = modifiedDatabases.get();
	if (localModifiedDatabases == nullptr) {
		localModifiedDatabases = new VectorSet<Database*>();

		modifiedDatabases.set(localModifiedDatabases);
	}

	return localModifiedDatabases;
}
