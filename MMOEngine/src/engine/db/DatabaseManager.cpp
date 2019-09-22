/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "DatabaseManager.h"
#include "engine/core/Core.h"

using namespace engine::db;
using namespace engine::db::berkeley;

void BerkeleyCheckpointTask::run() {
	manager->checkpoint();
}

//#ifdef VERSION_PUBLIC
//uint64 DatabaseManager::MAX_CACHE_SIZE = 500000000; // 500MB
//#else
uint64 DatabaseManager::MAX_CACHE_SIZE = -1;
//#endif

bool DatabaseManager::CONVERT_DATABASES = true;

DatabaseManager::DatabaseManager() : Logger("DatabaseManager") {
	loaded = false;

	setGlobalLogging(true);
	setLogLevel(LogLevel::LOG);

	databases.setNullValue(nullptr);
	databases.setAllowOverwriteInsertPlan();

	nameDirectory.setNullValue(-1);
	nameDirectory.setNoDuplicateInsertPlan();

	databaseDirectory = nullptr;

	checkpointTask = new BerkeleyCheckpointTask(this);

	lastTableID = 0;
	currentVersion = 0;

	managedObjectsWithHashCodeMembers = false;

	Core::initializeProperties("DatabaseManager");
	Core::initializeProperties("BerkeleyDB");
}

DatabaseManager::~DatabaseManager() {
	closeDatabases();

	if (databaseDirectory) {
		delete databaseDirectory;
		databaseDirectory = nullptr;
	}

	if (databaseEnvironment) {
		delete databaseEnvironment;
		databaseEnvironment = nullptr;
	}

	//checkpointTask->cancel();

	info("closed");
}

int DatabaseManager::failCheck() {
	return databaseEnvironment->failCheck();
}

void DatabaseManager::checkpoint() {
	CheckpointConfig checkpointConfig;
	checkpointConfig.setForce(true);

	databaseEnvironment->checkpoint(checkpointConfig);

	/*
	if (!checkpointTask->isScheduled())
		checkpointTask->schedule(CHECKPOINTTIME);
	 */
}

void DatabaseManager::openEnvironment() {
	MAX_CACHE_SIZE = Core::getIntProperty("DatabaseManager.maxCacheSize", -1);

	static const EnvironmentConfig config = [] () {
		EnvironmentConfig config;

		config.setRecover(Core::getIntProperty("DatabaseManager.recoverEnvironment", 1));
		config.setAllowCreate(true);
		config.setInitializeLocking(true);
		config.setInitializeLogging(true);
		config.setLockDown(Core::getIntProperty("DatabaseManager.lockDownMemory", 0));

		config.setLogAutoRemove(Core::getIntProperty("DatabaseManager.logAutoRemove", 1));
		config.setThreaded(Core::getIntProperty("DatabaseManager.threadedEnvironment", 1));
		config.setThreadCount(Core::getIntProperty("DatabaseManager.threadCount", 512));
		config.setRegister(Core::getIntProperty("DatabaseManager.registerEnvironment", 0));
		config.setPrivate(Core::getIntProperty("DatabaseManager.privateEnvironment", 0));
		config.setMultiVersionConcurrencyControl(Core::getIntProperty("BerkeleyDB.MVCC", 0));
		config.setDirectAccess(Core::getIntProperty("DatabaseManager.directAccess", 0));

		config.setTransactional(true);
		config.setInitializeCache(true);

		config.setMaxLogFileSize(Core::getIntProperty("DatabaseManager.logFileSize", 100 * 1024 * 1024));
		config.setLockDetectMode(LockDetectMode::RANDOM);

		return config;
	} ();

	try {
		databaseEnvironment = new Environment("databases", config);

		if (databaseEnvironment->failCheck() != 0) {
			fatal("Database environment crashed and cant continue, please run db_recover in the databases folder");
		}

	} catch (Exception& e) {
		error("Error opening environment...");

		fatal(e.getMessage());
	} catch (...) {
		fatal("unreported exception caught while trying to open berkeley environment ");
	}
}

void DatabaseManager::loadDatabases(bool truncateDatabases) {
	if (loaded)
		return;

	info("loading bdb databases...");

	try {
		openEnvironment();

		databaseDirectory = new LocalDatabase(this, "databases.db", false);

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
				TypeInfo<uint64>::parseFromBinaryStream(&currentVersion, &tableName);

				tableName.reset();
				inputKey.reset();
				continue;
			}

			if (tableKey == MANAGED_OBJECTS_HASHCODE_MEMBERS) {
				managedObjectsWithHashCodeMembers = true;

				tableName.reset();
				inputKey.reset();
				continue;
			}

			String dbName;
			dbName.parseFromBinaryStream(&tableName);

			uint32 tableType = (uint32)(tableKey >> 32);
			uint32 tableID = (uint32)((tableKey << 32) >> 32);

			LocalDatabase* db = nullptr;

			StringBuffer msg;

			bool compressionFlag = tableType & COMPRESSION_FLAG;

			if ((tableType & 0x7FFFFFFF) == LocalDatabase::LOCALDATABASE) {
				db = new LocalDatabase(this, String(dbName + ".db"), compressionFlag);

				if (truncateDatabases)
					msg << "truncating local database: ";
				else
					msg << "loading local database: ";
			} else if ((tableType & 0x7FFFFFFF) == LocalDatabase::OBJECTDATABASE) {
				db = new ObjectDatabase(this, String(dbName + ".db"), compressionFlag);

				if (truncateDatabases)
					msg << "truncating object database: ";
				else
					msg << "loading object database: ";
			} else if ((tableType & 0x7FFFFFFF) == LocalDatabase::INDEXDATABASE) {
				db = new IndexDatabase(this, String(dbName + ".db"), compressionFlag);

				if (truncateDatabases)
					msg << "truncating index database: ";
				else
					msg << "loading index database: ";
			}

			msg << dbName << " with id 0x" << hex << (uint16)tableID;

			if (compressionFlag)
				msg << " with compression enabled";

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
		fatal(e.getMessage());
	}

	if (!managedObjectsWithHashCodeMembers && CONVERT_DATABASES)
		convertDatabasesToHashCodeMembers();

	checkpoint();

	info("bdb databases loaded");
}

void DatabaseManager::convertDatabasesToHashCodeMembers() {
	info("converting database objects to new format", true);

	commitLocalTransaction();

	MAX_CACHE_SIZE = 500000000;

	Transaction* transaction = startTransaction();

	for (int i = 0; i < databases.size(); ++i) {
		LocalDatabase* db = databases.elementAt(i).getValue();

		if (!db->isObjectDatabase())
			continue;

		String name = db->getDatabaseName();

		info("converting database " + name, true);

		ObjectDatabase* objectDatabase = dynamic_cast<ObjectDatabase*>(db);

		ObjectDatabaseIterator iterator(transaction, db);

		ObjectInputStream data;
		uint64 key;

		//uint64 count = objectDatabase->getDatabaseHandle()->count(true, transaction);
		uint64 convertedCount = 0;

		//printf("total count %llu\n", count);

		while (iterator.getNextKeyAndValue(key, &data)) {
			ObjectOutputStream* newData = nullptr;

			++convertedCount;

			/*
			 * ObjectOutputStream* newData = Serializable::convertToHashCodeNameMembers(&data);

			objectDatabase->putData(key, newData, nullptr, transaction);
			 *
			 */

			try {
				newData = Serializable::convertToHashCodeNameMembers(&data);
			} catch (Exception& e) {
				//error("could not convert:" + data.toStringData());

				data.clear();
				continue;
			}

			objectDatabase->putData(key, newData, nullptr, transaction);

			data.clear();
		}

		printf("converted %llu\n", convertedCount);
	}

	setManagedObjectsWithHashCodeMembersFlag(transaction);

	commitLocalTransaction(transaction);
	commitTransaction(transaction);

	MAX_CACHE_SIZE = -1;

	info("finished converting data to new format", true);
}

void DatabaseManager::closeDatabases() {
	info("closing databases");

	if (!loaded)
		return;

	while (databases.size() > 0) {
		LocalDatabase* db = databases.remove(0).getValue();

		delete db;
	}

	if (databaseDirectory)  {
		delete databaseDirectory;
		databaseDirectory = nullptr;
	}

	closeEnvironment();

	loaded = false;
}

LocalDatabase* DatabaseManager::instantiateDatabase(const String& name, bool create, uint16 uniqueID,
		LocalDatabase::DatabaseType dbType, bool compression) {
	Locker _locker(this);

	if (uniqueID == 0xFFFF)
		uniqueID = getDatabaseID(name);

	LocalDatabase* db = databases.get(uniqueID);

	if (db != nullptr)
		return db;

	if (db == nullptr && !create)
		return nullptr;

	if (uniqueID == 0xFFFF)
		uniqueID = ++lastTableID;

	switch (dbType) {
		case LocalDatabase::LOCALDATABASE:
			db = new LocalDatabase(this, String(name + ".db"), compression);
			break;
		case LocalDatabase::OBJECTDATABASE:
			db = new ObjectDatabase(this, String(name + ".db"), compression);
			break;
		case LocalDatabase::INDEXDATABASE:
			db = new IndexDatabase(this, String(name + ".db"), compression);
			break;
		default:
			error("unkown database type for: " + name);
	}

	StringBuffer msg;
	msg << "trying to create database " << name << " with id 0x" << hex << uniqueID;

	if (compression)
		msg << " with compression enabled";

	info(msg);

	ObjectOutputStream* nameData = new ObjectOutputStream(20);

	String nm = name;
	nm.toBinaryStream(nameData);

	uint64 fullKey = 0;

	switch (dbType) {
		case LocalDatabase::LOCALDATABASE:
			fullKey += LocalDatabase::LOCALDATABASE;
			break;
		case LocalDatabase::OBJECTDATABASE:
			fullKey += LocalDatabase::OBJECTDATABASE;
			break;
		case LocalDatabase::INDEXDATABASE:
			fullKey += LocalDatabase::INDEXDATABASE;
			break;
	}

	if (compression)
		fullKey |= COMPRESSION_FLAG;

	fullKey = fullKey << 32;
	fullKey += uniqueID;

	ObjectOutputStream* stream = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&fullKey, stream);
	databaseDirectory->putData(stream, nameData);

	databases.put(uniqueID, db);
	nameDirectory.put(name, uniqueID);

	return db;
}

LocalDatabase* DatabaseManager::loadLocalDatabase(const String& name, bool create, uint16 uniqueID, bool compression, LocalDatabase::DatabaseType dbType) {
	return instantiateDatabase(name, create, uniqueID, dbType, compression);
}

IndexDatabase* DatabaseManager::loadIndexDatabase(const String& name, bool create, uint16 uniqueID, bool compression) {
	return dynamic_cast<IndexDatabase*>(instantiateDatabase(name, create, uniqueID, LocalDatabase::DatabaseType::INDEXDATABASE, compression));
}

engine::db::berkeley::Transaction* DatabaseManager::getReadLocalTransaction(bool abortPrevious) {
	if (!loaded)
		return nullptr;

	static const TransactionConfig config = []() {
		auto config = TransactionConfig::DEFAULT;

		static const bool mvcc  = Core::getIntProperty("BerkeleyDB.MVCC", 0);

		if (mvcc) {
			config.setSnapshot(true);
		} else {
			config.setReadUncommitted(true);
		}

		return config;
	} ();

	auto transaction = readLocalTransaction.get();

	if (transaction == nullptr) {
		transaction = startTransaction(config);

		readLocalTransaction.set(transaction);
	} else if (abortPrevious) {
		transaction->abort();

		transaction = startTransaction(config);

		readLocalTransaction.set(transaction);
       	}

	return transaction;
}

CurrentTransaction* DatabaseManager::getCurrentTransaction() {
	if (!loaded)
		return nullptr;

	CurrentTransaction* transaction = localTransaction.get();

	if (transaction == nullptr) {
		transaction = new CurrentTransaction(databaseEnvironment);

		localTransaction.set(transaction);
	}

	return transaction;
}

void DatabaseManager::addTemporaryObject(Object* object) {
//	if (this == nullptr)
//		return;

	if (!loaded)
		return;

	CurrentTransaction* trans = getCurrentTransaction();
	trans->addTemporaryObject(object);
}

void DatabaseManager::startLocalTransaction() {
	/*if (this == nullptr)
		return;

	if (!loaded)
		return;

	CurrentTransaction* trans = getCurrentTransaction();
	trans->startBerkeleyTransaction();*/
}

void DatabaseManager::abortLocalTransaction() {
	CurrentTransaction* transaction = localTransaction.get();

	if (transaction == nullptr)
		return;

	Vector<UpdateObject>* updateObjects = transaction->getUpdateVector();

	transaction->clearTemporaryObjects();

	if (updateObjects->size() == 0) {
		return;
	}

	for (int i = 0; i < updateObjects->size(); ++i) {
		UpdateObject* updateObject = &updateObjects->elementAt(i);

		Stream* stream = updateObject->stream;

		if (stream != nullptr)
			delete stream;
	}

	updateObjects->removeAll();
}

engine::db::berkeley::Transaction* DatabaseManager::startTransaction(const engine::db::berkeley::TransactionConfig& config) {
	Transaction* transaction = databaseEnvironment->beginTransaction(nullptr, config);

	return transaction;
}

int DatabaseManager::commitTransaction(engine::db::berkeley::Transaction* transaction) {
	int commitRet;

	/*if ((commitRet = transaction->commitNoSync()) != 0) {
		error("error commiting master berkeley transaction " + String::valueOf(db_strerror(commitRet)));
	}*/

	assert(transaction != nullptr);

	if ((commitRet = transaction->commitSync()) != 0) {
		error("error commiting master berkeley transaction " + String(db_strerror(commitRet)));
	}

	return commitRet;
}

void DatabaseManager::commitLocalTransaction(engine::db::berkeley::Transaction* masterTransaction) {
	//printf("commiting local transaction\n");

	if (!loaded)
		return;

	auto readTransaction = readLocalTransaction.get();

	if (readTransaction) {
		int res = 0;

		if ((res = readTransaction->commitNoSync()) != 0) {
			warning("error commiting berkeley transaction " + String(db_strerror(res)));
		}

		readLocalTransaction.set(nullptr);
	}

	CurrentTransaction* transaction = localTransaction.get();

	if (transaction == nullptr)
		return;

	Vector<UpdateObject>* updateObjects = transaction->getUpdateVector();

	if (updateObjects->size() == 0) {
		transaction->clearTemporaryObjects();

		return;
	}

	int iteration = 0;
	int ret = -1;
	int count = 0;

	Transaction* berkeleyTransaction = nullptr;

	do {
		ret = -1;

		berkeleyTransaction = databaseEnvironment->beginTransaction(masterTransaction);

		for (int i = 0; i < updateObjects->size(); ++i) {
			UpdateObject* updateObject = &updateObjects->elementAt(i);
			LocalDatabase* db = updateObject->database;
			Stream* id = updateObject->key;
			Stream* stream = updateObject->stream;

			++count;

			if (stream != nullptr) {
				ret = db->tryPutData(id, stream, berkeleyTransaction);

				if (ret == DB_LOCK_DEADLOCK) {
					berkeleyTransaction->abort();
					berkeleyTransaction = nullptr;

					warning("deadlock detected while trying to putData iterating time " + String::valueOf(iteration));
					break;
				} else if (ret != 0) {
					error("error while trying to putData :" + String(db_strerror(ret)));
				}

			} else {
				ret = db->tryDeleteData(id, berkeleyTransaction);

				if (ret == DB_LOCK_DEADLOCK) {
					berkeleyTransaction->abort();
					berkeleyTransaction = nullptr;

					warning("deadlock detected while trying to deleteData iterating time " + String::valueOf(iteration));
					break;
				} else if (ret != 0 && ret != DB_NOTFOUND) {
					StringBuffer msg;
					msg << "error while trying to deleteData :" << db_strerror(ret);
					error(msg.toString());
				}
			}

			Thread::yield();

			//printf("\r\tCommiting database updates: [%d / %d]", count, updateObjects->size());
		}


		++iteration;
	} while (ret == DB_LOCK_DEADLOCK && iteration < ObjectDatabase::DEADLOCK_MAX_RETRIES);

	if (iteration >= ObjectDatabase::DEADLOCK_MAX_RETRIES) {
		fatal("error exceeded deadlock retries shutting down");
	}

	/*if (count > 0)
		printf("\n");*/

	if (ret != 0 && ret != DB_NOTFOUND)
		berkeleyTransaction->abort();
	else {
		int commitRet = 0;

		//if ((commitRet = berkeleyTransaction->commitNoSync()) != 0) {
		if ((commitRet = berkeleyTransaction->commitSync()) != 0) {
			fatal("error commiting berkeley transaction " + String(db_strerror(commitRet)));
		}
	}

	transaction->clearTemporaryObjects();

	int oldSize = updateObjects->size();

	for (int i = 0; i < updateObjects->size(); ++i) {
		UpdateObject* updateObject = &updateObjects->elementAt(i);

		Stream* stream = updateObject->stream;

		if (stream != nullptr)
			delete stream;

		Stream* id = updateObject->key;

		if (id != nullptr)
			delete id;
	}

	updateObjects->removeAll(oldSize / 2, 50);
	transaction->resetCurrentSize();
}


void DatabaseManager::closeEnvironment() {
	if (!loaded || !databaseEnvironment)
		return;

	try {
		int ret = databaseEnvironment->close();

		if (ret != 0) {
			error("Error closing environment: ");
		} else {
			delete databaseEnvironment;

			databaseEnvironment = nullptr;
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

	name = db->getDatabaseName();
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

void DatabaseManager::setManagedObjectsWithHashCodeMembersFlag(engine::db::berkeley::Transaction* transaction) {
	if (managedObjectsWithHashCodeMembers)
		return;

	uint64 flag = 1;

	ObjectOutputStream* idData = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&flag, idData);

	ObjectOutputStream* stream = new ObjectOutputStream(8, 8);

	uint64 lastKey = MANAGED_OBJECTS_HASHCODE_MEMBERS;
	TypeInfo<uint64>::toBinaryStream(&lastKey, stream);

	databaseDirectory->putData(stream, idData, transaction);

	managedObjectsWithHashCodeMembers = true;
}

int DatabaseManager::compressDatabase(const String& name, engine::db::berkeley::Transaction* transaction) {
	String unconstName = name;

	uint16 id = nameDirectory.get(name);
	LocalDatabase* database = databases.get(id);

	if (database == nullptr) {
		error("no " + name + " database found to compress");
		return 1;
	}

	if (database->hasCompressionEnabled()) {
		error(name + " already has compression enabled");
		return 2;
	}

	database->compressDatabaseEntries(transaction);

	uint64 fullKey = 0;

	switch (database->getDatabaseType()) {
		case LocalDatabase::LOCALDATABASE:
			fullKey += LocalDatabase::LOCALDATABASE;
			break;
		case LocalDatabase::OBJECTDATABASE:
			fullKey += LocalDatabase::OBJECTDATABASE;
			break;
		case LocalDatabase::INDEXDATABASE:
			fullKey += LocalDatabase::INDEXDATABASE;
			break;
	}

	fullKey = fullKey << 32;
	fullKey += id;

	ObjectOutputStream* oldKeyStream = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&fullKey, oldKeyStream);

	databaseDirectory->deleteData(oldKeyStream, transaction);

	fullKey = 0;

	switch (database->getDatabaseType()) {
		case LocalDatabase::LOCALDATABASE:
			fullKey += LocalDatabase::LOCALDATABASE;
			break;
		case LocalDatabase::OBJECTDATABASE:
			fullKey += LocalDatabase::OBJECTDATABASE;
			break;
		case LocalDatabase::INDEXDATABASE:
			fullKey += LocalDatabase::INDEXDATABASE;
			break;
	}

	fullKey |= COMPRESSION_FLAG;

	fullKey = fullKey << 32;
	fullKey += id;

	ObjectOutputStream* stream = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&fullKey, stream);

	ObjectOutputStream* nameData = new ObjectOutputStream(20);
	unconstName.toBinaryStream(nameData);

	databaseDirectory->putData(stream, nameData, transaction);

	return 0;
}
