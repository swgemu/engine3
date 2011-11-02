#include "DatabaseManager.h"


using namespace engine::db;
using namespace engine::db::berkley;

void BerkeleyCheckpointTask::run() {
	manager->checkpoint();
}

DatabaseManager::DatabaseManager() : Logger("DatabaseManager") {
	loaded = false;

	setGlobalLogging(true);
	setInfoLogLevel();

	databases.setNullValue(NULL);
	databases.setNoDuplicateInsertPlan();

	nameDirectory.setNullValue(-1);
	nameDirectory.setNoDuplicateInsertPlan();

	databaseDirectory = NULL;

	checkpointTask = new BerkeleyCheckpointTask(this);

	lastTableID = 0;
	currentVersion = 0;
}

DatabaseManager::~DatabaseManager() {
	closeDatabases();

	delete databaseDirectory;
	databaseDirectory = NULL;

	delete databaseEnvironment;
	databaseEnvironment = NULL;

	checkpointTask->cancel();

	info("closed");
}

void DatabaseManager::checkpoint() {
	CheckpointConfig checkpointConfig;
	checkpointConfig.setForce(true);

	databaseEnvironment->checkpoint(checkpointConfig);

	if (!checkpointTask->isScheduled())
		checkpointTask->schedule(CHECKPOINTTIME);
}

void DatabaseManager::openEnvironment() {
	EnvironmentConfig config;
	config.setAllowCreate(true);
	config.setInitializeLocking(true);
	config.setInitializeLogging(true);
	config.setLogAutoRemove(true);
	config.setThreaded(true);
	config.setThreadCount(50);
	config.setTransactional(true);
	config.setInitializeCache(true);
	config.setMaxLogFileSize(1000 * 1000 * 100); // 100mb
	config.setLockDetectMode(LockDetectMode::RANDOM);
	//config.setLockDetectMode(LockDetectMode::YOUNGEST);

	try {
		databaseEnvironment = new Environment("databases", config);

		if (databaseEnvironment->failCheck() != 0) {
			error("Database environment crashed and cant continue, please run db_recover in the databases folder");
			exit(1);
		}

	} catch (Exception& e) {
		error("Error opening environment...");
		error(e.getMessage());
		exit(1);
	} catch (...) {
		error("unreported exception caught while trying to open berkeley environment ");
	}
}

void DatabaseManager::loadDatabases(bool truncateDatabases) {
	if (loaded)
		return;

	try {
		openEnvironment();

		databaseDirectory = new LocalDatabase(this, "databases.db");

		LocalDatabaseIterator iterator(databaseDirectory);

		uint64 tableKey;
		ObjectInputStream tableName(20, 20);
		ObjectInputStream inputKey(8, 8);

		while (iterator.getNextKeyAndValue(&inputKey, &tableName)) {
			tableKey = inputKey.readLong();

			if (tableKey == LASTOBJECTIDKEY) {
				tableName.reset();
				inputKey.reset();
				continue;
			}

			if (tableKey == VERSIONKEY) {
				currentVersion = tableKey;

				tableName.reset();
				inputKey.reset();
				continue;
			}

			String dbName;
			dbName.parseFromBinaryStream(&tableName);

			uint32 tableType = (uint32)(tableKey >> 32);
			uint32 tableID = (uint32)((tableKey << 32) >> 32);

			LocalDatabase* db = NULL;

			StringBuffer msg;

			if (tableType == LOCALDATABASE) {
				db = new LocalDatabase(this, String(dbName + ".db"));

				if (truncateDatabases)
					msg << "truncating local database: ";
				else
					msg << "loading local database: ";
			} else {
				db = new ObjectDatabase(this, String(dbName + ".db"));

				if (truncateDatabases)
					msg << "truncating local database: ";
				else
					msg << "loading object database: ";
			}

			msg << dbName << " with id 0x" << hex << (uint16)tableID;
			info(msg);

			if (truncateDatabases) {
				db->truncate();
			}

			databases.put((uint16)tableID, db);
			nameDirectory.put(dbName, tableID);

			if (tableID > lastTableID)
				lastTableID = tableID;

			tableName.reset();
			inputKey.reset();
		}

		StringBuffer msg;
		msg << "loaded database version: " << currentVersion;
		info(msg);

		loaded = true;
	} catch (Exception& e) {
		error(e.getMessage());

		assert(0 && "Database exception loading databases");
	}

	checkpoint();
}

void DatabaseManager::closeDatabases() {
	while (databases.size() > 0) {
		LocalDatabase* db = databases.remove(0).getValue();

		delete db;
	}

	delete databaseDirectory;
	databaseDirectory = NULL;

	closeEnvironment();

	loaded = false;
}

LocalDatabase* DatabaseManager::instantiateDatabase(const String& name, bool create, uint16 uniqueID, bool objectDatabase) {
	Locker _locker(this);

	if (uniqueID == 0xFFFF)
		uniqueID = getDatabaseID(name);

	LocalDatabase* db = databases.get(uniqueID);

	if (db != NULL)
		return db;

	if (db == NULL && !create)
		return NULL;

	if (uniqueID == 0xFFFF)
		uniqueID = ++lastTableID;

	if (objectDatabase)
		db = new ObjectDatabase(this, String(name + ".db"));
	else
		db = new LocalDatabase(this, String(name + ".db"));

	StringBuffer msg;
	msg << "trying to create database " << name << " with id 0x" << hex << uniqueID;
	info(msg);

	ObjectOutputStream* nameData = new ObjectOutputStream(20);

	String nm = name;
	nm.toBinaryStream(nameData);

	uint64 fullKey = 0;

	if (objectDatabase)
		fullKey += OBJECTDATABASE;
	else
		fullKey += LOCALDATABASE;

	fullKey = fullKey << 32;
	fullKey += uniqueID;

	ObjectOutputStream* stream = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&fullKey, stream);
	databaseDirectory->putData(stream, nameData);

	databases.put(uniqueID, db);
	nameDirectory.put(name, uniqueID);

	return db;
}

LocalDatabase* DatabaseManager::loadLocalDatabase(const String& name, bool create, uint16 uniqueID) {
	return instantiateDatabase(name, create, uniqueID, false);
}

CurrentTransaction* DatabaseManager::getCurrentTransaction() {
	if (!loaded)
		return NULL;

	CurrentTransaction* transaction = localTransaction.get();

	if (transaction == NULL) {
		transaction = new CurrentTransaction(databaseEnvironment);

		localTransaction.set(transaction);
	}

	return transaction;
}

void DatabaseManager::addTemporaryObject(Object* object) {
	if (this == NULL)
		return;

	if (!loaded)
		return;

	CurrentTransaction* trans = getCurrentTransaction();
	trans->addTemporaryObject(object);
}

void DatabaseManager::startLocalTransaction() {
	/*if (this == NULL)
		return;

	if (!loaded)
		return;

	CurrentTransaction* trans = getCurrentTransaction();
	trans->startBerkeleyTransaction();*/
}

void DatabaseManager::abortLocalTransaction() {
	CurrentTransaction* transaction = localTransaction.get();

	if (transaction == NULL)
		return;

	Vector<UpdateObject>* updateObjects = transaction->getUpdateVector();

	transaction->clearTemporaryObjects();

	if (updateObjects->size() == 0) {
		return;
	}

	for (int i = 0; i < updateObjects->size(); ++i) {
		UpdateObject* updateObject = &updateObjects->elementAt(i);

		Stream* stream = updateObject->stream;

		if (stream != NULL)
			delete stream;
	}

	updateObjects->removeAll();
}

engine::db::berkley::Transaction* DatabaseManager::startTransaction() {
	Transaction* transaction = databaseEnvironment->beginTransaction(NULL);

	return transaction;
}

int DatabaseManager::commitTransaction(engine::db::berkley::Transaction* transaction) {
	int commitRet;

	/*if ((commitRet = transaction->commitNoSync()) != 0) {
		error("error commiting master berkeley transaction " + String::valueOf(db_strerror(commitRet)));
	}*/

	if ((commitRet = transaction->commitSync()) != 0) {
		error("error commiting master berkeley transaction " + String::valueOf(db_strerror(commitRet)));
	}

	return commitRet;
}

void DatabaseManager::commitLocalTransaction(engine::db::berkley::Transaction* masterTransaction) {
	if (this == NULL)
		return;

	if (!loaded)
		return;

	CurrentTransaction* transaction = localTransaction.get();

	if (transaction == NULL)
		return;

	Vector<UpdateObject>* updateObjects = transaction->getUpdateVector();

	transaction->clearTemporaryObjects();

	if (updateObjects->size() == 0) {
		return;
	}

	int iteration = 0;
	int ret = -1;

	Transaction* berkeleyTransaction = NULL;

	do {
		ret = -1;

		berkeleyTransaction = databaseEnvironment->beginTransaction(masterTransaction);

		for (int i = 0; i < updateObjects->size(); ++i) {
			UpdateObject* updateObject = &updateObjects->elementAt(i);
			LocalDatabase* db = updateObject->database;
			Stream* id = updateObject->key;
			Stream* stream = updateObject->stream;

			if (stream != NULL) {
				ret = db->tryPutData(id, stream, berkeleyTransaction);

				if (ret == DB_LOCK_DEADLOCK) {
					berkeleyTransaction->abort();
					berkeleyTransaction = NULL;

					info("deadlock detected while trying to putData iterating time " + String::valueOf(iteration));
					break;
				} else if (ret != 0) {
					error("error while trying to putData :" + String::valueOf(db_strerror(ret)));
				}

			} else {
				ret = db->tryDeleteData(id, berkeleyTransaction);

				if (ret == DB_LOCK_DEADLOCK) {
					berkeleyTransaction->abort();
					berkeleyTransaction = NULL;

					info("deadlock detected while trying to deleteData iterating time " + String::valueOf(iteration));
					break;
				} else if (ret != 0 && ret != DB_NOTFOUND) {
					StringBuffer msg;
					msg << "error while trying to deleteData :" << db_strerror(ret);
					error(msg.toString());
				}
			}

			Thread::yield();
		}


		++iteration;
	} while (ret == DB_LOCK_DEADLOCK && iteration < ObjectDatabase::DEADLOCK_MAX_RETRIES);

	if (iteration >= ObjectDatabase::DEADLOCK_MAX_RETRIES) {
		error("error exceeded deadlock retries shutting down");
		exit(1);
	}

	if (ret != 0 && ret != DB_NOTFOUND)
		berkeleyTransaction->abort();
	else {
		int commitRet = 0;

		//if ((commitRet = berkeleyTransaction->commitNoSync()) != 0) {
		if ((commitRet = berkeleyTransaction->commitSync()) != 0) {
			error("error commiting berkeley transaction " + String::valueOf(db_strerror(commitRet)));
		}
	}

	int oldSize = updateObjects->size();

	for (int i = 0; i < updateObjects->size(); ++i) {
		UpdateObject* updateObject = &updateObjects->elementAt(i);

		Stream* stream = updateObject->stream;

		if (stream != NULL)
			delete stream;

		Stream* id = updateObject->key;

		if (id != NULL)
			delete id;
	}

	updateObjects->removeAll(oldSize, 50);
}


void DatabaseManager::closeEnvironment() {
	if (!loaded)
		return;

	try {
		int ret = databaseEnvironment->close();

		if (ret != 0) {
			error("Error closing environment: ");
		}

	} catch (Exception &e) {
		error("Error closing environment: ");
		error(e.getMessage());
	} catch (...) {
		error("unreported exception caught while trying to close environment");
	}
}

void DatabaseManager::getDatabaseName(uint16 tableID, String& name) {
	LocalDatabase* db = getDatabase(tableID);

	db->getDatabaseName(name);
}

void DatabaseManager::updateLastUsedObjectID(uint64 id) {
	ObjectOutputStream* idData = new ObjectOutputStream(8);
	TypeInfo<uint64>::toBinaryStream(&id, idData);

	ObjectOutputStream* stream = new ObjectOutputStream(8, 8);

	uint64 lastKey = LASTOBJECTIDKEY;
	TypeInfo<uint64>::toBinaryStream(&lastKey, stream);

	databaseDirectory->putData(stream, idData);
}

uint64 DatabaseManager::getLastUsedObjectID() {
	ObjectInputStream idData(8, 8);
	ObjectOutputStream keyData(8, 8);

	uint64 lastKey = LASTOBJECTIDKEY;
	TypeInfo<uint64>::toBinaryStream(&lastKey, &keyData);

	if (databaseDirectory->getData(&keyData, &idData))
		return 0;

	uint64 id;

	TypeInfo<uint64>::parseFromBinaryStream(&id, &idData);

	return id;
}

void DatabaseManager::updateCurrentVersion(uint64 version) {
	if (currentVersion == version)
		return;

	currentVersion = version;

	ObjectOutputStream* idData = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&version, idData);

	ObjectOutputStream* stream = new ObjectOutputStream(8, 8);

	uint64 lastKey = VERSIONKEY;
	TypeInfo<uint64>::toBinaryStream(&lastKey, stream);

	databaseDirectory->putData(stream, idData);
}
