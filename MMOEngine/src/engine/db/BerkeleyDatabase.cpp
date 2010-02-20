/*
 * BerkeleyDatabase.cpp
 *
 *  Created on: 13/02/2010
 *      Author: victor
 */

#include "BerkeleyDatabase.h"
#include "DatabaseException.h"
#include "Cursor.h"

CheckpointConfig CheckpointConfig::DEFAULT;
DatabaseConfig DatabaseConfig::DEFAULT;

BerkeleyDatabase::BerkeleyDatabase(const String& filename, const String& databaseName, const DatabaseConfig& databaseConfig) {
	BerkeleyDatabase::databaseConfig = databaseConfig;
	BerkeleyDatabase::filename = filename;
	BerkeleyDatabase::databaseName = databaseName;

	int ret = db_create(&dbp, NULL, 0);

	if (ret != 0)
		throw DatabaseException("unable to create database handle with ret code " + String::valueOf(ret));

	ret = dbp->open(dbp,        /* DB structure pointer */
			NULL,       /* Transaction pointer */
			filename.toCharArray(), /* On-disk file that holds the database. */
			NULL,       /* Optional logical database name */
			this->databaseConfig.getDatabaseType(),   /* Database access method */
			this->databaseConfig.getDatabaseFlags(),      /* Open flags */
			0);         /* File mode (using defaults) */

	if (ret != 0) {
		/* Error handling goes here */
		throw DatabaseException("unable to open database with ret code " + String::valueOf(ret));
	}

}

BerkeleyDatabase::BerkeleyDatabase(Environment* env, Transaction* txn, const String& filename, const String& databaseName, const DatabaseConfig& databaseConfig) {
	environment = env;
	BerkeleyDatabase::databaseConfig = databaseConfig;
	BerkeleyDatabase::filename = filename;
	BerkeleyDatabase::databaseName = databaseName;

	this->databaseConfig.setTransactional(true);

	DB_TXN* transaction = NULL;

	if (txn != NULL)
		transaction = txn->getDBTXN();

	DB_ENV* dbenvp = env->getDatabaseEnvironmentHandle();

	int ret = db_create(&dbp, dbenvp, 0);

	if (ret != 0)
		throw DatabaseException("unable to create database handle with ret code " + String::valueOf(ret));

	ret = dbp->open(dbp,        /* DB structure pointer */
			transaction,       /* Transaction pointer */
			filename.toCharArray(), /* On-disk file that holds the database. */
			NULL,       /* Optional logical database name */
			this->databaseConfig.getDatabaseType(),   /* Database access method */
			this->databaseConfig.getDatabaseFlags(),      /* Open flags */
			0);         /* File mode (using defaults) */

	if (ret != 0) {
		/* Error handling goes here */
		throw DatabaseException("unable to open database with ret code " + String::valueOf(ret));
	}
}


BerkeleyDatabase::~BerkeleyDatabase() {
	if (dbp != NULL) {
	    int ret = dbp->close(dbp, 0);
	    dbp = NULL;
	}
}

int BerkeleyDatabase::get(Transaction* txn, DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode) {
	DB_TXN* transaction = NULL;

	if (txn != NULL)
		transaction = txn->getDBTXN();

	return dbp->get(dbp, transaction, key->getDBT(), data->getDBT(), lockMode);
}

int BerkeleyDatabase::put(Transaction* txn, DatabaseEntry* key, DatabaseEntry* data) {
	DB_TXN* transaction = NULL;

	if (txn != NULL)
		transaction = txn->getDBTXN();

	return dbp->put(dbp, transaction, key->getDBT(), data->getDBT(), 0);
}

int BerkeleyDatabase::del(Transaction* txn, DatabaseEntry* key) {
	DB_TXN* transaction = NULL;

	if (txn != NULL)
		transaction = txn->getDBTXN();

	return dbp->del(dbp, transaction, key->getDBT(), 0);
}

Cursor* BerkeleyDatabase::openCursor(Transaction* txn, const CursorConfig& config) {
	uint32 flags = config.getFlags();

	DB_TXN* transaction = NULL;

	if (txn != NULL)
		transaction = txn->getDBTXN();

	DBC* cursorp = NULL;

	int ret = dbp->cursor(dbp, transaction, &cursorp, flags);

	if (ret != 0) {
		throw DatabaseException("error opening cursor with ret " + String::valueOf(ret));
	}

	Cursor* cursor = new Cursor(this, cursorp, config);
	return cursor;
}

void BerkeleyDatabase::sync() {
	int ret = dbp->sync(dbp, 0);

	if (ret != 0)
		throw DatabaseException("error while synching to disk with ret " + String::valueOf(ret));
}

void BerkeleyDatabase::close(bool noSync) {
	uint32 flags = 0;

	if (noSync == true)
		flags |= DB_NOSYNC;

	if (dbp != NULL) {
		int ret = dbp->close(dbp, flags);

		if (ret != 0)
			throw DatabaseException("error while closing database with ret " + String::valueOf(ret));

		dbp = NULL;
	}
}

int BerkeleyDatabase::truncate(Transaction* txn, bool countRecords) {
	uint32 recordCount = 0;
	DB_TXN* txnid = NULL;

	if (txn != NULL)
		txnid = txn->getDBTXN();

	int ret = dbp->truncate(dbp, txnid, &recordCount, 0);

	if (ret != 0)
		throw DatabaseException("error while truncating table with ret " + String::valueOf(ret));

	if (countRecords)
		return recordCount;
	else
		return -1;
}
