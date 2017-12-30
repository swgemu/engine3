/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "MysqlDatabaseManager.h"

void MysqlDatabaseManager::commitModifiedDatabases() {
	VectorSet<MySqlDatabase*>* localModifiedDatabases = getLocalModifiedDatabases();
	for (int i = 0; i < localModifiedDatabases->size(); ++i) {
		MySqlDatabase* database = localModifiedDatabases->get(i);

		database->commit();
	}
}

void MysqlDatabaseManager::rollbackModifiedDatabases() {
	VectorSet<MySqlDatabase*>* localModifiedDatabases = getLocalModifiedDatabases();
	for (int i = 0; i < localModifiedDatabases->size(); ++i) {
		MySqlDatabase* database = localModifiedDatabases->get(i);

		database->rollback();
	}
}

void MysqlDatabaseManager::addModifiedDatabase(MySqlDatabase* database) {
	VectorSet<MySqlDatabase*>* localModifiedDatabases = getLocalModifiedDatabases();
	localModifiedDatabases->add(database);
}

VectorSet<MySqlDatabase*>* MysqlDatabaseManager::getLocalModifiedDatabases() {
	VectorSet<MySqlDatabase*>* localModifiedDatabases = modifiedDatabases.get();
	if (localModifiedDatabases == nullptr) {
		localModifiedDatabases = new VectorSet<MySqlDatabase*>();

		modifiedDatabases.set(localModifiedDatabases);
	}

	return localModifiedDatabases;
}
