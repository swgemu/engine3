/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#include "ObjectDatabase.h"

#include "ObjectDatabaseManager.h"

#include "engine/core/Task.h"
#include "engine/core/TaskWorkerThread.h"

#include "engine/core/Core.h"

#ifdef CXX11_COMPILER
#include <chrono>
#endif

using namespace engine::db;
using namespace engine::db::berkeley;

ObjectDatabase::ObjectDatabase(DatabaseManager* dbEnv, const String& dbFileName, bool compression, DatabaseType databaseType)
	: LocalDatabase(dbEnv, dbFileName, compression, databaseType) {

	dbType = LocalDatabase::OBJECTDATABASE;

	setLogLevel(LogLevel::LOG);
	setLoggingName("ObjectDatabase " + dbFileName);
}

int ObjectDatabase::getData(uint64 objKey, ObjectInputStream* objectData, uint32 lockMode, bool compressed, bool readThreadLocalTransaction) {
	if (objKey == 0) {
		return DB_NOTFOUND;
	}

	int ret = 0;

	DatabaseEntry key, data;
	key.setData(&objKey, sizeof(uint64));

	int i = 0;

	Transaction* transaction = nullptr;
	TransactionConfig cfg = TransactionConfig::DEFAULT;

	static const bool mvcc = Core::getIntProperty("BerkeleyDB.MVCC", 0);

	if (lockMode == LockMode::READ_UNCOMMITED && !mvcc) {
		cfg.setReadUncommitted(true);
	} else if (mvcc) {
		cfg.setSnapshot(true);

		lockMode = LockMode::DEFAULT;
	}

	cfg.setNoSync(true);

	Timer profiler;
	profiler.start();

	bool abortPreviousLocalTransaction = false;
	const static bool disableReadLocalTransactions = Core::getIntProperty("DatabaseManager.disableReadLocalTx", 1);

	if (disableReadLocalTransactions) {
		readThreadLocalTransaction = false;
	}

	do {
		if (readThreadLocalTransaction) {
			transaction = ObjectDatabaseManager::instance()->getReadLocalTransaction(abortPreviousLocalTransaction);
		} else {
			transaction = environment->beginTransaction(nullptr, cfg);
		}

		ret = getDatabaseHandle()->get(transaction, &key, &data, lockMode);

		if (ret == DB_LOCK_DEADLOCK && !readThreadLocalTransaction) {
			info() << "deadlock detected in ObjectDatabse::get.. retrying iteration " << i;

			transaction->abort();
			transaction = nullptr;
		}

		abortPreviousLocalTransaction = true;

		++i;
	} while (ret == DB_LOCK_DEADLOCK && i < DEADLOCK_MAX_RETRIES);

	if (ret == 0) {
		if (!compression || compressed)
			objectData->writeStream((const char*) data.getData(), data.getSize());
		else
			uncompress(data.getData(), data.getSize(), objectData);

		objectData->reset();
	} else if (ret != DB_NOTFOUND) {
		error() << "error in ObjectDatabase::getData ret " << db_strerror(ret);

		if (!readThreadLocalTransaction && transaction != nullptr)
			transaction->abort();

		throw DatabaseException("error in ObjectDatabase::getData ret " + String(db_strerror(ret)));
	}

	if (!readThreadLocalTransaction && transaction != nullptr) {
		transaction->commitNoSync();
	}

	uint64 elapsedTime = profiler.stop();

	auto thread = Thread::getCurrentThread();
	auto worker = thread ? thread->asTaskWorkerThread() : nullptr;

	if (worker) {
		worker->addBDBReadStats(databaseFileName, elapsedTime);
	}

	static const bool slowLog = Core::getIntProperty("BerkeleyDB.slowQueryLog", 0);

	if (!slowLog) {
		return ret;
	}

	static const uint64 slowTaskLogMs = Core::getIntProperty("BerkeleyDB.slowQueryLogMs", 40);
	static const String slowTaskFilename = Core::getProperty("BerkeleyDB.slowQueryLogFile", "");

	auto elapsedTimeMs = elapsedTime / 1000000;

	if (elapsedTimeMs < slowTaskLogMs) {
		return ret;
	}

	static const auto ignoreDbs = Core::getPropertyVector("BerkeleyDB.slowQueryLogIgnore");
	const auto& dbName = getDatabaseFileName();

	if (ignoreDbs.contains(dbName)) {
		return ret;
	}

	StringBuffer stream;
	stream << dbName << " took ";

	if (elapsedTimeMs == 0) {
		stream << elapsedTime << "ns";
	} else {
		stream << elapsedTimeMs << "ms";
	}

	stream << " to lookup 0x" << hex << objKey;

	if (!slowTaskFilename.isEmpty()) {
		static Logger customLogger = []() {
			Logger log("BerkeleyDBSlowQueryLogger");

			log.setGlobalLogging(false);
			log.setFileLogger(slowTaskFilename, true);
			log.setLogLevelToFile(false);

			return log;
		} ();

		customLogger.log(stream);
	} else {
		warning(stream);
	}

	return ret;
}

int ObjectDatabase::getDataNoTx(uint64 objKey, ObjectInputStream* objectData, uint32 lockMode, bool compressed) {
	int ret = 0;

	DatabaseEntry key, data;

	key.setData(&objKey, sizeof(uint64));

	int i = 0;

	ret = getDatabaseHandle()->get(nullptr, &key, &data, lockMode);

	if (ret == 0) {
		if (!compression || compressed)
			objectData->writeStream((const char*) data.getData(), data.getSize());
		else
			uncompress(data.getData(), data.getSize(), objectData);

		objectData->reset();
	} else if (ret != DB_NOTFOUND) {
		error() << "error in ObjectDatabase::getData ret " << db_strerror(ret);

		throw DatabaseException("error in ObjectDatabase::getData ret " + String(db_strerror(ret)));
	}

	return ret;
}

int ObjectDatabase::tryPutData(uint64 objKey, Stream* stream, engine::db::berkeley::Transaction* transaction) {
	//Locker locker(&Environment::guard);

	int ret = -1;

	DatabaseEntry key, data;
	key.setData(&objKey, sizeof(uint64));

	//data.setData(stream->getBuffer(), stream->size());

	if (!compression) {
		data.setData(stream->getBuffer(), stream->size());

		ret = getDatabaseHandle()->put(transaction, &key, &data);
	} else {
		Stream* compressed = compress(stream);

		data.setData(compressed->getBuffer(), compressed->size());

		ret = getDatabaseHandle()->put(transaction, &key, &data);

		delete compressed;
	}

	return ret;
}

int ObjectDatabase::tryDeleteData(uint64 objKey, engine::db::berkeley::Transaction* transaction) {
	//Locker locker(&Environment::guard);

	int ret = -1;

	DatabaseEntry key;
	key.setData(&objKey, sizeof(uint64));

	ret = getDatabaseHandle()->del(transaction, &key);

	return ret;
}

int ObjectDatabase::putData(uint64 objKey, Stream* objectData, Object* obj, engine::db::berkeley::Transaction* masterTransaction) {
	ObjectDatabaseManager* databaseManager = ObjectDatabaseManager::instance();

	CurrentTransaction* transaction = databaseManager->getCurrentTransaction();

	ObjectOutputStream* key = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&objKey, key);

	uint64 currentSize = transaction->addUpdateObject(key, objectData, this, obj);

	if (currentSize > DatabaseManager::MAX_CACHE_SIZE) {
		ObjectDatabaseManager::instance()->commitLocalTransaction(masterTransaction);
	}

	return 0;
}

int ObjectDatabase::deleteData(uint64 objKey, engine::db::berkeley::Transaction* masterTransaction) {
	ObjectOutputStream* key = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&objKey, key);

	return LocalDatabase::deleteData(key, masterTransaction);

	/*StringBuffer msg;
	msg << "added to deleteData objid" << hex << objKey;
	info(msg);*/
}

bool ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, ObjectInputStream* data, uint32 lockMode, bool compressed) {
	try {
		Locker locker(&Environment::guard);

		if (cursor->getNext(&this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

		key = *reinterpret_cast<uint64*>(this->key.getData());

		if (!localDatabase->hasCompressionEnabled() || compressed)
			data->writeStream((char*)this->data.getData(), this->data.getSize());
		else
			LocalDatabase::uncompress(this->data.getData(), this->data.getSize(), data);

		data->reset();

	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return true;
}


int ObjectDatabaseIterator::getNextKeyAndValueMultiple(berkeley::DatabaseEntry& data, uint32 lockMode) {
	int res = 0;

	Locker locker(&Environment::guard);

	try {
		if ((res = cursor->getNextMultiple(&this->key, &data, lockMode)) != 0) {
			/*this->key.setData(nullptr, 0);
			  this->data.setData(nullptr, 0);*/
			return res;
		}

	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return res;
}

bool ObjectDatabaseIterator::getPrevKeyAndValue(uint64& key, ObjectInputStream* data, uint32 lockMode) {
	ObjectInputStream stream(8, 8);

	bool res = LocalDatabaseIterator::getPrevKeyAndValue(&stream, data, lockMode);

	if (res == true) {
		key = stream.readLong();
	}

	return res;
}

bool ObjectDatabaseIterator::getPrevKey(uint64& key, uint32 lockMode) {
	ObjectInputStream stream(8, 8);

	bool res = LocalDatabaseIterator::getPrevKey(&stream, lockMode);

	if (res == true) {
		key = stream.readLong();
	}

	return res;
}

bool ObjectDatabaseIterator::getLastKey(uint64& key, uint32 lockMode) {
	ObjectInputStream stream(8, 8);

	bool res = LocalDatabaseIterator::getLastKey(&stream, lockMode);

	if (res == true) {
		key = stream.readLong();
	}

	return res;
}

bool ObjectDatabaseIterator::getNextKey(uint64& key, uint32 lockMode) {
	Locker locker(&Environment::guard);

	ObjectInputStream stream(8, 8);

	bool res = LocalDatabaseIterator::getNextKey(&stream, lockMode);

	if (res == true) {
		key = stream.readLong();
	}

	return res;
}

bool ObjectDatabaseIterator::getSearchKey(uint64 key, ObjectInputStream* data, uint32 lockMode) {
	ObjectOutputStream stream;
	stream.writeLong(key);

	return LocalDatabaseIterator::getSearchKey(&stream, data, lockMode);
}

bool ObjectDatabaseIterator::getSearchKeyRange(uint64& key, ObjectInputStream* data, uint32 lockMode) {
	Packet stream;
	stream.writeLong(key);

	bool res = LocalDatabaseIterator::getSearchKeyRange(&stream, data, lockMode);
	stream.reset();

	if (res == true) {
		key = stream.readLong();
	}

	return res;
}
