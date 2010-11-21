#include "ObjectDatabase.h"

#include "ObjectDatabaseManager.h"

using namespace engine::db;
using namespace engine::db::berkley;

ObjectDatabase::ObjectDatabase(DatabaseManager* dbEnv, const String& dbFileName)
	: LocalDatabase(dbEnv, dbFileName) {

	setLoggingName("ObjectDatabase " + dbFileName);
}

int ObjectDatabase::getData(uint64 objKey, ObjectInputStream* objectData) {
	int ret = 0;

	DatabaseEntry key, data;

	key.setData(&objKey, sizeof(uint64));

	int i = 0;

	Transaction* transaction = NULL;

	do {
		ret  = -1;
		transaction = environment->beginTransaction(NULL);

		ret = objectsDatabase->get(transaction, &key, &data, LockMode::READ_UNCOMMITED);

		if (ret == DB_LOCK_DEADLOCK) {
			info("deadlock detected in ObjectDatabse::get.. retrying iteration " + String::valueOf(i), true);
			transaction->abort();
			transaction = NULL;
		}

		++i;
	} while (ret == DB_LOCK_DEADLOCK && i < DEADLOCK_MAX_RETRIES);

	if (ret == 0) {
		objectData->writeStream((const char*) data.getData(), data.getSize());

		objectData->reset();
	} else if (ret != DB_NOTFOUND) {
		error("error in ObjectDatabase::getData ret " + String::valueOf(db_strerror(ret)));

		if (transaction != NULL)
			transaction->abort();

		throw DatabaseException("error in ObjectDatabase::getData ret " + String(db_strerror(ret)));
	}

	if (transaction != NULL) {
		transaction->commit();
	}


	return ret;
}

int ObjectDatabase::tryPutData(uint64 objKey, Stream* stream, engine::db::berkley::Transaction* transaction) {
	int ret = -1;

	DatabaseEntry key, data;
	key.setData(&objKey, sizeof(uint64));
	data.setData(stream->getBuffer(), stream->size());

	ret = objectsDatabase->put(transaction, &key, &data);

	return ret;
}

int ObjectDatabase::tryDeleteData(uint64 objKey, engine::db::berkley::Transaction* transaction) {
	int ret = -1;

	DatabaseEntry key;
	key.setData(&objKey, sizeof(uint64));

	ret = objectsDatabase->del(transaction, &key);

	return ret;
}

int ObjectDatabase::putData(uint64 objKey, Stream* objectData, Object* obj) {
	ObjectDatabaseManager* databaseManager = ObjectDatabaseManager::instance();

	CurrentTransaction* transaction = databaseManager->getCurrentTransaction();

	ObjectOutputStream* key = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&objKey, key);
	transaction->addUpdateObject(key, objectData, this, obj);

	return 0;
}

int ObjectDatabase::deleteData(uint64 objKey) {
	ObjectOutputStream* key = new ObjectOutputStream(8, 8);
	TypeInfo<uint64>::toBinaryStream(&objKey, key);

	return LocalDatabase::deleteData(key);

	/*StringBuffer msg;
	msg << "added to deleteData objid" << hex << objKey;
	info(msg.toString(), true);*/
}

bool ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, ObjectInputStream* data) {
	ObjectInputStream stream(8, 8);

	bool res = LocalDatabaseIterator::getNextKeyAndValue(&stream, data);

	if (res == true) {
		key = stream.readLong();
	}

	return res;
}

bool ObjectDatabaseIterator::getNextKey(uint64& key) {
	ObjectInputStream stream(8, 8);

	bool res = LocalDatabaseIterator::getNextKey(&stream);

	if (res == true) {
		key = stream.readLong();
	}

	return res;
}
