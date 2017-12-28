/*
 * Cursor.cpp
 *
 *  Created on: 15/02/2010
 *      Author: victor
 */

#include "engine/db/DatabaseException.h"

#include "Cursor.h"
#include "DatabaseEntry.h"

using namespace engine::db::berkley;

CursorConfig CursorConfig::DEFAULT;

Cursor::Cursor(BerkeleyDatabase* database, DBC* dbcursor, const CursorConfig& config) {
	Cursor::database = database;
	Cursor::config = config;

	dbc = dbcursor;
}

Cursor::~Cursor() {
	if (dbc != NULL) {
		close();
	}
}

int Cursor::close() {
	int ret = dbc->close(dbc);

	dbc = NULL;

	return ret;
}

int Cursor::del(uint32 flags) {
	return dbc->del(dbc, flags);
}

Cursor* Cursor::dup(bool samePosition) {
	uint32 flags = 0;

	if (samePosition)
		flags = DB_POSITION;

	DBC* dbdup = NULL;

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
	return dbc->get(dbc, NULL, data->getDBT(), DB_GET_RECNO | lockMode);
}

int Cursor::put(DatabaseEntry* key, DatabaseEntry* data, uint32 flags) {
	return dbc->put(dbc, key->getDBT(), data->getDBT(), flags);
}

int Cursor::putCurrent(DatabaseEntry* data) {
	return dbc->put(dbc, NULL, data->getDBT(), DB_CURRENT);
}



