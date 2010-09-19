#include "ObjectDatabaseManager.h"


using namespace engine::db;
using namespace engine::db::berkley;

void BerkeleyCheckpointTask::run() {
	manager->checkpoint();
}

ObjectDatabaseManager::ObjectDatabaseManager() : Logger("ObjectDatabaseManager") {

	loaded = false;

	setGlobalLogging(true);
	setLogging(false);

	databases.setNullValue(NULL);
	databases.setNoDuplicateInsertPlan();

	nameDirectory.setNullValue(-1);
	nameDirectory.setNoDuplicateInsertPlan();

	databaseDirectory = NULL;

	checkpointTask = new BerkeleyCheckpointTask(this);

	lastTableID = 0;

	openEnvironment();
	//loadDatabases();

	//checkpoint();
}

ObjectDatabaseManager::~ObjectDatabaseManager() {
	closeDatabases();

	delete databaseDirectory;
	databaseDirectory = NULL;

	closeEnvironment();

	delete databaseEnvironment;
	databaseEnvironment = NULL;

	checkpointTask->cancel();

	info("closed", true);
}

void ObjectDatabaseManager::checkpoint() {
	CheckpointConfig checkpointConfig;
	checkpointConfig.setForce(true);

	databaseEnvironment->checkpoint(checkpointConfig);

	if (!checkpointTask->isScheduled())
		checkpointTask->schedule(CHECKPOINTTIME);
}

void ObjectDatabaseManager::openEnvironment() {
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
	//config.setLockDetectMode(LockDetectMode::RANDOM);
	config.setLockDetectMode(LockDetectMode::YOUNGEST);

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

void ObjectDatabaseManager::loadDatabases() {
	if (loaded)
		return;

	databaseDirectory = new ObjectDatabase(this, "databases.db");

	ObjectDatabaseIterator iterator(databaseDirectory);

	uint64 tableID;
	ObjectInputStream tableName(20);

	while (iterator.getNextKeyAndValue(tableID, &tableName)) {
		if (tableID == LASTOBJECTIDKEY) {
			tableName.reset();
			continue;
		}

		String dbName;
		dbName.parseFromBinaryStream(&tableName);

		ObjectDatabase* db = new ObjectDatabase(this, String(dbName + ".db"));

		StringBuffer msg;
		msg << "loading database:" << dbName << " with id 0x" << hex << (uint16)tableID;
		info(msg.toString(), true);

		databases.put((uint16)tableID, db);
		nameDirectory.put(dbName, tableID);

		if (tableID > lastTableID)
			lastTableID = tableID;

		tableName.reset();
	}

	loaded = true;

	checkpoint();
}

void ObjectDatabaseManager::closeDatabases() {
	while (databases.size() > 0) {
		ObjectDatabase* db = databases.remove(0).getValue();

		delete db;
	}

	delete databaseDirectory;
	databaseDirectory = NULL;

	loaded = false;
}

ObjectDatabase* ObjectDatabaseManager::loadDatabase(const String& name, bool create, uint16 uniqueID) {
	Locker _locker(this);

	if (uniqueID == 0xFFFF)
		uniqueID = getDatabaseID(name);

	ObjectDatabase* db = databases.get(uniqueID);

	if (db != NULL)
		return db;

	if (db == NULL && !create)
		return NULL;

	if (uniqueID == 0xFFFF)
		uniqueID = ++lastTableID;

	db = new ObjectDatabase(this, String(name + ".db"));

	StringBuffer msg;
	msg << "trying to create database " << name << " with id 0x" << hex << uniqueID;
	info(msg.toString(), true);

	ObjectOutputStream nameData(20);

	String nm = name;
	nm.toBinaryStream(&nameData);

	databaseDirectory->putData((uint64)uniqueID, &nameData, NULL);

	databases.put(uniqueID, db);
	nameDirectory.put(name, uniqueID);

	return db;
}

CurrentTransaction* ObjectDatabaseManager::getCurrentTransaction() {
	if (!loaded)
		return NULL;

	CurrentTransaction* transaction = localTransaction.get();

	if (transaction == NULL) {
		transaction = new CurrentTransaction(databaseEnvironment);

		localTransaction.set(transaction);
	}

	return transaction;
}

void ObjectDatabaseManager::startLocalTransaction() {
	if (this == NULL)
		return;

	if (!loaded)
		return;

	CurrentTransaction* trans = getCurrentTransaction();
	trans->startBerkeleyTransaction();
}

void ObjectDatabaseManager::commitLocalTransaction() {
	if (this == NULL)
		return;

	if (!loaded)
		return;

	CurrentTransaction* transaction = localTransaction.get();

	if (transaction == NULL)
		return;

	Vector<UpdateObject>* updateObjects = transaction->getUpdateVector();
	Transaction* berkeleyTransaction = transaction->getBerkeleyTransaction();

	if (berkeleyTransaction == NULL)
		return;

	if (updateObjects->size() == 0) {
		berkeleyTransaction->abort();
		transaction->clearBerkeleyTransaction();
		return;
	}

	int iteration = 0;
	int ret = -1;


	do {
		ret = -1;

		if (iteration != 0)
			berkeleyTransaction = databaseEnvironment->beginTransaction(NULL);

		for (int i = 0; i < updateObjects->size(); ++i) {
			UpdateObject* updateObject = &updateObjects->elementAt(i);
			ObjectDatabase* db = updateObject->database;
			uint64 id = updateObject->objectid;
			Stream* stream = updateObject->stream;

			if (stream != NULL) {
				ret = db->tryPutData(id, stream, berkeleyTransaction);

				if (ret == DB_LOCK_DEADLOCK) {
					berkeleyTransaction->abort();
					info("deadlock detected while trying to putData iterating time " + String::valueOf(iteration), true);
					break;
				} else if (ret != 0) {
					error("error while trying to putData :" + String::valueOf(db_strerror(ret)));
				}

			} else {
				ret = db->tryDeleteData(id, berkeleyTransaction);

				if (ret == DB_LOCK_DEADLOCK) {
					berkeleyTransaction->abort();
					info("deadlock detected while trying to deleteData iterating time " + String::valueOf(iteration), true);
					break;
				} else if (ret != 0 && ret != DB_NOTFOUND) {
					StringBuffer msg;
					msg << "error while trying to deleteData :" << db_strerror(ret);
					error(msg.toString());
				}
			}
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

		if ((commitRet = berkeleyTransaction->commitNoSync()) != 0) {
			error("error commiting berkeley transaction " + String::valueOf(db_strerror(ret)));
		}
	}

	for (int i = 0; i < updateObjects->size(); ++i) {
		UpdateObject* updateObject = &updateObjects->elementAt(i);

		Stream* stream = updateObject->stream;

		if (stream != NULL)
			delete stream;
	}

	updateObjects->removeAll();

	transaction->clearBerkeleyTransaction();

}


void ObjectDatabaseManager::closeEnvironment() {
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

void ObjectDatabaseManager::getDatabaseName(uint16 tableID, String& name) {
	ObjectDatabase* db = getDatabase(tableID);

	db->getDatabaseName(name);
}

void ObjectDatabaseManager::updateLastUsedObjectID(uint64 id) {
	ObjectOutputStream idData(8);

	TypeInfo<uint64>::toBinaryStream(&id, &idData);

	databaseDirectory->putData(LASTOBJECTIDKEY, &idData, NULL);
}

uint64 ObjectDatabaseManager::getLastUsedObjectID() {
	ObjectInputStream idData(8);

	if (databaseDirectory->getData(LASTOBJECTIDKEY, &idData))
		return 0;

	uint64 id;

	TypeInfo<uint64>::parseFromBinaryStream(&id, &idData);

	return id;
}
