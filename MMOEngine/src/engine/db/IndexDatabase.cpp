/**
 * @author      : theanswer (theanswer@hyperv)
 * @file        : IndexDatabase
 * @created     : Saturday Aug 03, 2019 08:12:13 UTC
 */

#include "IndexDatabase.h"

IndexDatabase::IndexDatabase() {
	dbType = LocalDatabase::INDEXDATABASE;

	setLogLevel(LogLevel::LOG);
	setLoggingName("IndexDatabase");
}

IndexDatabase::IndexDatabase(DatabaseManager* dbEnv, const String& dbFileName, bool compression)
	: ObjectDatabase(dbEnv, dbFileName, compression, INDEXDATABASE) {

	dbType = LocalDatabase::INDEXDATABASE;

	setLogLevel(LogLevel::LOG);
	setLoggingName("IndexDatabase " + dbFileName);
}

 bool IndexDatabaseIterator::setKeyAndGetValue(uint64 key, uint64& primaryKey, ObjectInputStream* data,
	       uint32 lockMode, bool compressed) {
	try {
		Locker locker(&berkeley::Environment::guard);

		berkeley::DatabaseEntry searchKey;
		searchKey.setData(&key, sizeof(uint64));

		if (cursor->pGetSet(&searchKey, &this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

		primaryKey = *reinterpret_cast<uint64*>(this->key.getData());

		if (data) {
			if (!localDatabase->hasCompressionEnabled() || compressed)
				data->writeStream((char*)this->data.getData(), this->data.getSize());
			else
				LocalDatabase::uncompress(this->data.getData(), this->data.getSize(), data);

			data->reset();
		}

	} catch (...) {
		error("unreported exception caught in IndexDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return true;
 }

 bool IndexDatabaseIterator::getNextKeyAndValue(uint64 key, uint64& primaryKey, ObjectInputStream* data,
	       uint32 lockMode, bool compressed) {
	try {
		Locker locker(&berkeley::Environment::guard);

		berkeley::DatabaseEntry searchKey;
		searchKey.setData(&key, sizeof(uint64));

		if (cursor->pGetNextDup(&searchKey, &this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

		primaryKey = *reinterpret_cast<uint64*>(this->key.getData());

		if (data) {
			if (!localDatabase->hasCompressionEnabled() || compressed)
				data->writeStream((char*)this->data.getData(), this->data.getSize());
			else
				LocalDatabase::uncompress(this->data.getData(), this->data.getSize(), data);

			data->reset();
		}
	} catch (...) {
		error("unreported exception caught in IndexDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return true;
 }
