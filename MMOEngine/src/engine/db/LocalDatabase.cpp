/*
 * LocalDatabase.cpp
 *
 *  Created on: 11/11/2010
 *      Author: victor
 */

#include "LocalDatabase.h"
#include "DatabaseManager.h"
#include "ObjectDatabaseManager.h"

using namespace engine::db;
using namespace engine::db::berkley;

LocalDatabase::LocalDatabase(DatabaseManager* dbEnv, const String& dbFileName) {
	environment = dbEnv->getBerkeleyEnvironment();

	setLoggingName("LocalDatabase " + dbFileName);
	setGlobalLogging(true);
	setInfoLogLevel();

	databaseFileName = dbFileName;

	objectsDatabase = NULL;

	//setFileLogger("log/berkeley.log");

	openDatabase();
}

LocalDatabase::~LocalDatabase() {
	closeDatabase();

	delete objectsDatabase;
	objectsDatabase = NULL;
}

void LocalDatabase::openDatabase() {
	DatabaseConfig config;
	config.setThreaded(true);
	//config.setTransactional(true);
	config.setAllowCreate(true);
	config.setType(DatabaseType::HASH);
	config.setReadUncommited(true);

	LocalDatabase::openDatabase(config);
}

int LocalDatabase::sync() {
	objectsDatabase->sync();

	return 0;
}

int LocalDatabase::truncate() {
	try {
		objectsDatabase->truncate(NULL, false);

		info("database truncated");

	} catch (Exception &e) {
		error("Error truncating database (" + databaseFileName + "):");
		error(e.getMessage());
	} catch (...) {
		error("unreported exception caught while trying to truncate berkeley DB ");
	}

	return 0;
}

void LocalDatabase::openDatabase(const DatabaseConfig& config) {
	try {

		Transaction* transaction = environment->beginTransaction(NULL);

		objectsDatabase = environment->openDatabase(transaction, databaseFileName, "", config);

		int ret = 0;

		if ((ret = transaction->commit()) != 0) {
			error(String::valueOf(db_strerror(ret)));
		}


	} catch (Exception& e) {
		error(e.getMessage());
		exit(1);
	}
}

void LocalDatabase::closeDatabase() {
	try {

		objectsDatabase->close(true);

		info("database closed");

	} catch (Exception &e) {
		error("Error closing database (" + databaseFileName + "):");
		error(e.getMessage());
	} catch (...) {
		error("unreported exception caught while trying to open berkeley DB ");
	}
}

int LocalDatabase::getData(Stream* inputKey, ObjectInputStream* objectData) {
	int ret = 0;

	DatabaseEntry key, data;

	key.setData(inputKey->getBuffer(), inputKey->size());

	int i = 0;

	Transaction* transaction = NULL;

	do {
		ret  = -1;
		transaction = environment->beginTransaction(NULL);

		ret = objectsDatabase->get(transaction, &key, &data, LockMode::READ_UNCOMMITED);

		if (ret == DB_LOCK_DEADLOCK) {
			info("deadlock detected in LocalDatabase::get.. retrying iteration " + String::valueOf(i));

			transaction->abort();
			transaction = NULL;
		}

		++i;
	} while (ret == DB_LOCK_DEADLOCK && i < DEADLOCK_MAX_RETRIES);

	if (ret == 0) {
		objectData->writeStream((const char*) data.getData(), data.getSize());

		objectData->reset();
	} else if (ret != DB_NOTFOUND) {
		error("error in LocalDatabase::getData ret " + String::valueOf(db_strerror(ret)));

		if (transaction != NULL)
			transaction->abort();

		throw DatabaseException("error in LocalDatabase::getData ret " + String(db_strerror(ret)));
	}

	if (transaction != NULL) {
		transaction->commit();
	}

	return ret;
}

//stream will be deleted
int LocalDatabase::putData(Stream* inputKey, Stream* stream) {
	ObjectDatabaseManager* databaseManager = ObjectDatabaseManager::instance();

	CurrentTransaction* transaction = databaseManager->getCurrentTransaction();

	transaction->addUpdateObject(inputKey, stream, this, NULL);

	return 0;
}

int LocalDatabase::deleteData(Stream* inputKey) {
	ObjectDatabaseManager* databaseManager = ObjectDatabaseManager::instance();

	CurrentTransaction* transaction = databaseManager->getCurrentTransaction();

	transaction->addDeleteObject(inputKey, this);

	/*StringBuffer msg;
		msg << "added to deleteData objid" << hex << objKey;
		info(msg);*/

	return 0;
}

int LocalDatabase::tryPutData(Stream* inputKey, Stream* stream, engine::db::berkley::Transaction* transaction) {
	int ret = -1;

	DatabaseEntry key, data;
	key.setData(inputKey->getBuffer(), inputKey->size());
	data.setData(stream->getBuffer(), stream->size());

	ret = objectsDatabase->put(transaction, &key, &data);

	return ret;
}

int LocalDatabase::tryDeleteData(Stream* inputKey, engine::db::berkley::Transaction* transaction) {
	int ret = -1;

	DatabaseEntry key;
	key.setData(inputKey->getBuffer(), inputKey->size());

	ret = objectsDatabase->del(transaction, &key);

	return ret;
}


LocalDatabaseIterator::LocalDatabaseIterator(LocalDatabase* database, bool useCurrentThreadTransaction)
	: Logger("LocalDatabaseIterator") {

	databaseHandle = database->getDatabaseHandle();

	Transaction* txn = NULL;//ObjectDatabaseManager::instance()->getLocalTransaction();

/*	if (useCurrentThreadTransaction)
		txn = ObjectDatabaseManager::instance()->getLocalTransaction();*/

	cursor = databaseHandle->openCursor(txn);

	data.setReuseBuffer(true);
	key.setReuseBuffer(true);
}

LocalDatabaseIterator::LocalDatabaseIterator(BerkeleyDatabase* dbHandle)
	: Logger("LocalDatabaseIterator") {
	databaseHandle = dbHandle;
	Transaction* txn = NULL; // ObjectDatabaseManager::instance()->getLocalTransaction();
	cursor = databaseHandle->openCursor(txn);

	data.setReuseBuffer(true);
	key.setReuseBuffer(true);
}

LocalDatabaseIterator::~LocalDatabaseIterator() {
	closeCursor();
}

void LocalDatabaseIterator::resetIterator() {
	if (cursor != NULL) {
		cursor->close();
		delete cursor;
	}


	Transaction* txn = NULL;//
	//Transaction* txn = ObjectDatabaseManager::instance()->getLocalTransaction();

	cursor = databaseHandle->openCursor(txn);
}

bool LocalDatabaseIterator::getNextKeyAndValue(ObjectInputStream* key, ObjectInputStream* data) {
	try {
		if (cursor->getNext(&this->key, &this->data, LockMode::READ_UNCOMMITED) != 0) {
			/*this->key.setData(NULL, 0);
			this->data.setData(NULL, 0);*/
			return false;
		}

		key->writeStream((char*)this->key.getData(), this->key.getSize());
		data->writeStream((char*)this->data.getData(), this->data.getSize());

		data->reset();
		key->reset();

	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return true;
}

bool LocalDatabaseIterator::getNextValue(ObjectInputStream* data) {
	try {
		if (cursor->getNext(&this->key, &this->data, LockMode::READ_UNCOMMITED) != 0) {
			/*this->key.setData(NULL, 0);
			this->data.setData(NULL, 0);*/
			return false;
		}

		data->writeStream((char*)this->data.getData(), this->data.getSize());

		data->reset();

	} catch(Exception &e) {
		error("Error in ObjectDatabaseIterator::getNextValue(String& data)");
		error(e.getMessage());
	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextValue(String& data)");
	}

	return true;
}

bool LocalDatabaseIterator::getNextKey(ObjectInputStream* key) {
	try {
		if (cursor->getNext(&this->key, &this->data, LockMode::READ_UNCOMMITED) != 0) {
			/*this->key.setData(NULL, 0);
			this->data.setData(NULL, 0);*/
			return false;
		}

		key->writeStream((char*)this->key.getData(), this->key.getSize());

		key->reset();

	}  catch(Exception &e) {
		error("Error in ObjectDatabaseIterator::getNextKey");
		error(e.getMessage());
	} catch (...) {
		error("unreported exception caught  in ObjectDatabaseIterator::getNextKey(uint64& key) ");
	}

	return true;
}

