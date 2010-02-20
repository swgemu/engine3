/*
 * DatabaseEntry.cpp
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#include "DatabaseEntry.h"

DatabaseEntry::DatabaseEntry() {
	memset(&databaseEntry, 0, sizeof(DBT));

	databaseEntry.flags |= DB_DBT_MALLOC;
}

DatabaseEntry::DatabaseEntry(byte* data, int size) {
	memset(&databaseEntry, 0, sizeof(DBT));

	databaseEntry.size = size;
	databaseEntry.data = data;

	setUserBuffer(size, true);
}

DatabaseEntry::~DatabaseEntry() {
	if (!(databaseEntry.flags & DB_DBT_USERMEM) && databaseEntry.data != NULL)
		free(databaseEntry.data);
}

/*DatabaseEntry::DatabaseEntry(byte* data, int offset, int size) {
	memset(&databaseEntry, 0, sizeof(DBT));

	databaseEntry.size = size;
	databaseEntry.data = data + offset;
}*/

DatabaseEntry::DatabaseEntry(Stream* stream) {
	memset(&databaseEntry, 0, sizeof(DBT));

	databaseEntry.size = stream->size();
	databaseEntry.data = stream->getBuffer();

	setUserBuffer(stream->size(), true);
}

void DatabaseEntry::setData(void* addr, int size) {
	databaseEntry.data = addr;
	databaseEntry.size = size;

	setUserBuffer(size, true);
}
