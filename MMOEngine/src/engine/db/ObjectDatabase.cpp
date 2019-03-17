#include "ObjectDatabase.h"

#include "ObjectDatabaseManager.h"

#include "engine/core/Task.h"
#include "engine/core/TaskWorkerThread.h"

#ifdef CXX11_COMPILER
#include <chrono>
#endif


using namespace engine::db;
using namespace engine::db::berkley;

ObjectDatabase::ObjectDatabase(DatabaseManager* dbEnv, const String& dbFileName, bool compression)
	: LocalDatabase(dbEnv, dbFileName, compression) {

	setLogLevel(LogLevel::LOG);
	setLoggingName("ObjectDatabase " + dbFileName);
}

int ObjectDatabase::getData(uint64 objKey, ObjectInputStream* objectData, uint32 lockMode) {
	int ret = 0;

	DatabaseEntry key, data;

	key.setData(&objKey, sizeof(uint64));

	int i = 0;

	Transaction* transaction = nullptr;

	TransactionConfig cfg = TransactionConfig::DEFAULT;
	cfg.setNoSync(true);

	if (lockMode == LockMode::READ_UNCOMMITED)
		cfg.setReadUncommitted(true);

#ifdef COLLECT_TASKSTATISTICS
#ifdef CXX11_COMPILER
	auto start = std::chrono::high_resolution_clock::now();
#else
	Timer executionTimer(Time::MONOTONIC_TIME);

	executionTimer.start();
#endif
#endif

	do {
		ret  = -1;
		transaction = environment->beginTransaction(nullptr, cfg);

		ret = objectsDatabase->get(transaction, &key, &data, lockMode);

		if (ret == DB_LOCK_DEADLOCK) {
			info("deadlock detected in ObjectDatabse::get.. retrying iteration " + String::valueOf(i));
			transaction->abort();
			transaction = nullptr;
		}

		++i;
	} while (ret == DB_LOCK_DEADLOCK && i < DEADLOCK_MAX_RETRIES);

	if (ret == 0) {
		if (!compression)
			objectData->writeStream((const char*) data.getData(), data.getSize());
		else
			uncompress(data.getData(), data.getSize(), objectData);

		objectData->reset();
	} else if (ret != DB_NOTFOUND) {
		error("error in ObjectDatabase::getData ret " + String::valueOf(db_strerror(ret)));

		if (transaction != nullptr)
			transaction->abort();

		throw DatabaseException("error in ObjectDatabase::getData ret " + String(db_strerror(ret)));
	}

	if (transaction != nullptr) {
		transaction->commitNoSync();
	}

#ifdef COLLECT_TASKSTATISTICS
#ifdef CXX11_COMPILER
	auto end = std::chrono::high_resolution_clock::now();

	uint64 elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
#else
	uint64 elapsedTime = executionTimer.stop();
#endif

	Thread* thread = Thread::getCurrentThread();
	TaskWorkerThread* worker = thread ? thread->asTaskWorkerThread() : nullptr;

	if (worker) {
		worker->addBDBReadStats(databaseFileName, elapsedTime);
	}
#endif

	return ret;
}

int ObjectDatabase::getDataNoTx(uint64 objKey, ObjectInputStream* objectData, uint32 lockMode) {
	int ret = 0;

	DatabaseEntry key, data;

	key.setData(&objKey, sizeof(uint64));

	int i = 0;

	ret = objectsDatabase->get(nullptr, &key, &data, lockMode);

	if (ret == 0) {
		if (!compression)
			objectData->writeStream((const char*) data.getData(), data.getSize());
		else
			uncompress(data.getData(), data.getSize(), objectData);

		objectData->reset();
	} else if (ret != DB_NOTFOUND) {
		error("error in ObjectDatabase::getData ret " + String::valueOf(db_strerror(ret)));

		throw DatabaseException("error in ObjectDatabase::getData ret " + String(db_strerror(ret)));
	}

	return ret;
}

int ObjectDatabase::tryPutData(uint64 objKey, Stream* stream, engine::db::berkley::Transaction* transaction) {
	int ret = -1;

	DatabaseEntry key, data;
	key.setData(&objKey, sizeof(uint64));

	//data.setData(stream->getBuffer(), stream->size());

	if (!compression) {
		data.setData(stream->getBuffer(), stream->size());

		ret = objectsDatabase->put(transaction, &key, &data);
	} else {
		Stream* compressed = compress(stream);

		data.setData(compressed->getBuffer(), compressed->size());

		ret = objectsDatabase->put(transaction, &key, &data);

		delete compressed;
	}

	return ret;
}

int ObjectDatabase::tryDeleteData(uint64 objKey, engine::db::berkley::Transaction* transaction) {
	int ret = -1;

	DatabaseEntry key;
	key.setData(&objKey, sizeof(uint64));

	ret = objectsDatabase->del(transaction, &key);

	return ret;
}

int ObjectDatabase::putData(uint64 objKey, Stream* objectData, Object* obj, engine::db::berkley::Transaction* masterTransaction) {
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

int ObjectDatabase::deleteData(uint64 objKey, engine::db::berkley::Transaction* masterTransaction) {
	ObjectOutputStream* key = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&objKey, key);

	return LocalDatabase::deleteData(key, masterTransaction);

	/*StringBuffer msg;
	msg << "added to deleteData objid" << hex << objKey;
	info(msg);*/
}

bool ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, ObjectInputStream* data, uint32 lockMode, bool compressed) {
	try {
		if (cursor->getNext(&this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		key = *reinterpret_cast<uint64*>(this->key.getData());

		if (!localDatabase->hasCompressionEnabled() || !compressed)
			data->writeStream((char*)this->data.getData(), this->data.getSize());
		else
			LocalDatabase::uncompress(this->data.getData(), this->data.getSize(), data);

		data->reset();

	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return true;
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
