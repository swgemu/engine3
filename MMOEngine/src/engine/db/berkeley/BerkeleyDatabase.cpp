/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * BerkeleyDatabase.cpp
 *
 *  Created on: 13/02/2010
 *      Author: victor
 */

#include "engine/db/DatabaseException.h"

#include "BerkeleyDatabase.h"

#include "engine/db/ObjectDatabaseManager.h"

#include "engine/core/Core.h"

using namespace engine::db::berkeley;

CheckpointConfig CheckpointConfig::DEFAULT;
DatabaseConfig DatabaseConfig::DEFAULT;

BerkeleyDatabase::BerkeleyDatabase(const String& filename, const String& databaseName, const DatabaseConfig& databaseConfig) {
	BerkeleyDatabase::databaseConfig = databaseConfig;
	BerkeleyDatabase::filename = filename;
	BerkeleyDatabase::databaseName = databaseName;

	int ret = db_create(&dbp, nullptr, 0);

	if (ret != 0)
		throw DatabaseException("unable to create database handle with ret code " + String::valueOf(ret));

	static const int pageSize = Core::getIntProperty("BerkeleyDB.dbPageSize", 16384);

	dbp->set_pagesize(dbp, pageSize);
	dbp->set_flags(dbp, DB_CHKSUM);

	if (databaseConfig.hasDuplicates()) {
		dbp->set_flags(dbp, DB_DUP | DB_DUPSORT);
	}

	ret = dbp->open(dbp,        /* DB structure pointer */
			nullptr,       /* Transaction pointer */
			filename.toCharArray(), /* On-disk file that holds the database. */
			nullptr,       /* Optional logical database name */
			this->databaseConfig.getDatabaseType(),   /* Database access method */
			this->databaseConfig.getDatabaseFlags(),      /* Open flags */
			0);         /* File mode (using defaults) */

	if (ret != 0) {
		/* Error handling goes here */
		throw DatabaseException("unable to open database with ret code " + String(db_strerror(ret)));
	}

}

BerkeleyDatabase::BerkeleyDatabase(Environment* env, Transaction* txn, const String& filename, const String& databaseName, const DatabaseConfig& databaseConfig) {
	environment = env;
	BerkeleyDatabase::databaseConfig = databaseConfig;
	BerkeleyDatabase::filename = filename;
	BerkeleyDatabase::databaseName = databaseName;

	//this->databaseConfig.setTransactional(true);

	DB_TXN* transaction = nullptr;

	if (txn != nullptr)
		transaction = txn->getDBTXN();

	DB_ENV* dbenvp = env->getDatabaseEnvironmentHandle();

	int ret = db_create(&dbp, dbenvp, 0);

	if (ret != 0)
		throw DatabaseException("unable to create database handle with ret code " + String::valueOf(ret));

	const static int pageSize = Core::getIntProperty("BerkeleyDB.dbPageSize", 16384);

	dbp->set_pagesize(dbp, pageSize);
	dbp->set_flags(dbp, DB_CHKSUM);

	if (databaseConfig.hasDuplicates()) {
		dbp->set_flags(dbp, DB_DUP | DB_DUPSORT);
	}

	ret = dbp->open(dbp,        /* DB structure pointer */
			transaction,       /* Transaction pointer */
			filename.toCharArray(), /* On-disk file that holds the database. */
			nullptr,       /* Optional logical database name */
			this->databaseConfig.getDatabaseType(),   /* Database access method */
			this->databaseConfig.getDatabaseFlags(),      /* Open flags */
			0);         /* File mode (using defaults) */

	if (ret != 0) {
		/* Error handling goes here */
		throw DatabaseException("unable to open database with ret code " + String::valueOf(ret));
	}
}


BerkeleyDatabase::~BerkeleyDatabase() {
//	Logger::console.info("deleting bdbd", true);

//these will be freed when the environment dies;
/*	if (dbp != nullptr) {
	    int ret = dbp->close(dbp, 0);
	    dbp = nullptr;
	}
	*/
}

int BerkeleyDatabase::get(Transaction* txn, DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode) {
	DB_TXN* transaction = nullptr;

	if (txn != nullptr)
		transaction = txn->getDBTXN();

	return dbp->get(dbp, transaction, key->getDBT(), data->getDBT(), lockMode);
}

int BerkeleyDatabase::getMultiple(Transaction* txn, DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode) {
	DB_TXN* transaction = nullptr;

	if (txn != nullptr)
		transaction = txn->getDBTXN();

	return dbp->get(dbp, transaction, key->getDBT(), data->getDBT(), lockMode | DB_MULTIPLE);
}

int BerkeleyDatabase::pGetMultiple(Transaction* txn, DatabaseEntry* key, DatabaseEntry* primaryKey, DatabaseEntry* data, uint32 lockMode) {
	DB_TXN* transaction = nullptr;

	if (txn != nullptr)
		transaction = txn->getDBTXN();

	return dbp->pget(dbp, transaction, key->getDBT(), primaryKey->getDBT(), data->getDBT(), lockMode | DB_MULTIPLE);
}

int BerkeleyDatabase::put(Transaction* txn, DatabaseEntry* key, DatabaseEntry* data) {
	DB_TXN* transaction = nullptr;

	if (txn != nullptr)
		transaction = txn->getDBTXN();

	return dbp->put(dbp, transaction, key->getDBT(), data->getDBT(), 0);
}

int BerkeleyDatabase::del(Transaction* txn, DatabaseEntry* key) {
	DB_TXN* transaction = nullptr;

	if (txn != nullptr)
		transaction = txn->getDBTXN();

	return dbp->del(dbp, transaction, key->getDBT(), 0);
}

Cursor* BerkeleyDatabase::openCursor(Transaction* txn, const CursorConfig& config) {
	uint32 flags = config.getFlags();

	DB_TXN* transaction = nullptr;

	if (txn != nullptr)
		transaction = txn->getDBTXN();

	DBC* cursorp = nullptr;

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

	if (dbp != nullptr) {
		int ret = dbp->close(dbp, flags);

		if (ret != 0)
			throw DatabaseException(String("error while closing database with ret ") + db_strerror(ret));

		dbp = nullptr;
	}
}

int BerkeleyDatabase::truncate(Transaction* txn, bool countRecords) {
	uint32 recordCount = 0;
	DB_TXN* txnid = nullptr;

	if (txn != nullptr)
		txnid = txn->getDBTXN();

	int ret = dbp->truncate(dbp, txnid, &recordCount, 0);

	if (ret != 0)
		throw DatabaseException("error while truncating table with ret " + String::valueOf(ret));

	if (countRecords)
		return recordCount;
	else
		return -1;
}

int BerkeleyDatabase::compact(Transaction* txn, DB_COMPACT* compactData, uint32 flags) {
	DB_TXN* txnid = nullptr;

	if (txn != nullptr)
		txnid = txn->getDBTXN();

	return dbp->compact(dbp, txnid, nullptr, nullptr, compactData, flags, nullptr);
}

uint64 BerkeleyDatabase::count(bool forceCalculation, Transaction* txn) {
	int ret = -1;
	uint32 flags = 0;

	DB_TXN* txnid = nullptr;

	if (txn != nullptr)
		txnid = txn->getDBTXN();

	if (!forceCalculation)
		flags |= DB_FAST_STAT;

	switch (databaseConfig.getDatabaseType()) {
	case DatabaseType::HASH: {
		DB_HASH_STAT *sp;

		if (!(ret = dbp->stat(dbp, txnid, &sp, flags))) {
			uint64 val = sp->hash_nkeys;

			free(sp);

			return val;
		}

		break;
	}
	case DatabaseType::BTREE:
	case DatabaseType::RECNO: {
		DB_BTREE_STAT* sp;

		if (!(ret = dbp->stat(dbp, txnid, &sp, flags))) {
			uint64 val = sp->bt_nkeys;

			free(sp);

			return val;
		}

		break;
	}
	case DatabaseType::QUEUE: {
		DB_QUEUE_STAT* sp;

		if (!(ret = dbp->stat(dbp, txnid, &sp, flags))) {
			uint64 val = sp->qs_nkeys;

			free(sp);

			return val;
		}

		break;
	}
	default:
		throw DatabaseException("error in BerkeleyDatabase::count unknown database type");
	}

	if (ret != 0) {
		throw DatabaseException("error BerkeleyDatabase::count with ret " + String::valueOf(ret));
	}

	return -1;
}

int BerkeleyDatabase::associate(Transaction* txn, BerkeleyDatabase* secondary, int (*callback)(DB *secondary,
				    const DBT *key, const DBT *data, DBT *result), u_int32_t flags) {
	//Logger::console.info("associating db", true);

	return dbp->associate(dbp, txn ? txn->getDBTXN() : nullptr, secondary->getDatabaseHandle(), callback, flags);
}
