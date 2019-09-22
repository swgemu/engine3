/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Cursor.cpp
 *
 *  Created on: 15/02/2010
 *      Author: victor
 */

#include "engine/db/DatabaseException.h"

#include "Cursor.h"
#include "DatabaseEntry.h"
#include "engine/db/berkeley/Environment.h"

using namespace engine::db::berkeley;

CursorConfig CursorConfig::DEFAULT;

Cursor::Cursor(BerkeleyDatabase* database, DBC* dbcursor, const CursorConfig& config) {
	Cursor::database = database;
	Cursor::config = config;

	dbc = dbcursor;
}

Cursor::~Cursor() {
	if (dbc != nullptr) {
		close();
	}
}

int Cursor::close() {
	Locker locker(&Environment::guard);

	int ret = dbc->close(dbc);

	dbc = nullptr;

	return ret;
}

int Cursor::del(uint32 flags) {
	return dbc->del(dbc, flags);
}

Cursor* Cursor::dup(bool samePosition) {
	uint32 flags = 0;

	if (samePosition)
		flags = DB_POSITION;

	DBC* dbdup = nullptr;

	int res = dbc->dup(dbc, &dbdup, flags);

	if (res != 0) {
		throw DatabaseException("error duplicating cursor with ret " + String::valueOf(res));
	}

	Cursor* cursor = new Cursor(database, dbdup, config);
	return cursor;
}

int Cursor::get(DatabaseEntry* key, DatabaseEntry* data, uint32 flags) {
	return dbc->get(dbc, key->getDBT(), data->getDBT(), flags);
}

int Cursor::getRecordNumber(DatabaseEntry* data, uint32 lockMode) {
	return dbc->get(dbc, nullptr, data->getDBT(), DB_GET_RECNO | lockMode);
}

int Cursor::pget(DatabaseEntry* key, DatabaseEntry* primaryKey, DatabaseEntry* data, uint32 flags) {
	return dbc->pget(dbc, key->getDBT(), primaryKey->getDBT(), data->getDBT(), flags);
}

int Cursor::put(DatabaseEntry* key, DatabaseEntry* data, uint32 flags) {
	return dbc->put(dbc, key->getDBT(), data->getDBT(), flags);
}

int Cursor::putCurrent(DatabaseEntry* data) {
	return dbc->put(dbc, nullptr, data->getDBT(), DB_CURRENT);
}



