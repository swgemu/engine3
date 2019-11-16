/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * LocalDatabase.cpp
 *
 *  Created on: 11/11/2010
 *      Author: victor
 */

#include <zlib.h>

#include "LocalDatabase.h"
#include "DatabaseManager.h"
#include "ObjectDatabaseManager.h"
#include "engine/core/Core.h"

using namespace engine::db;
using namespace engine::db::berkeley;

namespace LD3Ns {
	static bool shutdown = false;

	static void runThreadLocalDtor(void* value) {
		shutdown = true;

		if (value) {
			delete reinterpret_cast<engine::db::berkeley::BerkeleyDatabase*>(value);
		}
	}
}

int LocalDatabase::DEADLOCK_MAX_RETRIES = 1000;

LocalDatabase::LocalDatabase(DatabaseManager* dbEnv, const String& dbFileName, bool compression, DatabaseType databaseType) :
		objectsDatabase() {
	environment = dbEnv->getBerkeleyEnvironment();

	setLoggingName("LocalDatabase " + dbFileName);
	setGlobalLogging(true);
	setLogLevel(LogLevel::LOG);

	databaseFileName = dbFileName;

	objectsDatabase = nullptr;

	this->compression = compression;
	this->dbType = databaseType;

	//setFileLogger("log/berkeley.log");

	openDatabase();
}

LocalDatabase::~LocalDatabase() {
	//delete objectsDatabase;
	//objectsDatabase = nullptr;
}

engine::db::berkeley::BerkeleyDatabase* LocalDatabase::getDatabaseHandle() {
	auto db = objectsDatabase.get();

	if (db == nullptr) {
		if (!LD3Ns::shutdown) {
			openDatabase();
		}

		db = objectsDatabase.get();
	} else {
		auto last = lastAssociation.get();
		auto latest = associations.get();

		if (!LD3Ns::shutdown && (last != latest)) {
			for (uint64 i = last; i < latest; i++) {
				auto& index = secondaryIndexes.get(i);

				fatal(db->associate(nullptr, index.first->getDatabaseHandle(), index.second,
				     DB_CREATE) == 0, "error could not associate secondary index");
			}

			lastAssociation.set(latest);
		}
	}

	return db;
}

void LocalDatabase::openDatabase() {
	static const DatabaseConfig config = [] () {
		DatabaseConfig config;

		DEADLOCK_MAX_RETRIES = Core::getIntProperty("BerkeleyDB.deadlockRetries", DEADLOCK_MAX_RETRIES);

		//config.setThreaded(true);
		//config.setTransactional(true);
		config.setAllowCreate(true);

		/*typedef enum {
		  DB_BTREE=1,
		  DB_HASH=2,
		  DB_HEAP=6,
		  DB_RECNO=3,
		  DB_QUEUE=4,
		  DB_UNKNOWN=5
		  } DBTYPE;*/

		config.setType(static_cast<DBTYPE>(Core::getIntProperty("BerkeleyDB.type", static_cast<int>(berkeley::DatabaseType::HASH))));
		static const bool mvcc = Core::getIntProperty("BerkeleyDB.MVCC", 0);
		config.setReadUncommited(!mvcc);
		config.setMultiVersionConcurrencyControl(mvcc);
		config.setReadOnly(Core::getIntProperty("BerkeleyDB.readOnly", 0));

		return config;
	} ();

	DatabaseConfig copy = config;

	if (dbType == INDEXDATABASE) {
		copy.setType(DB_BTREE);
		copy.setDuplicates(true);
	}

	LocalDatabase::openDatabase(copy);
}

int LocalDatabase::sync() {
	Locker locker(&Environment::guard);

	getDatabaseHandle()->sync();

	return 0;
}

int LocalDatabase::truncate() {
	Locker locker(&Environment::guard);

	try {
		getDatabaseHandle()->truncate(nullptr, false);

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
	if (objectsDatabase.get() != nullptr) {
		return;
	}

	try {
		Transaction* transaction = environment->beginTransaction(nullptr);

		//info("opening database type: " + String::valueOf(static_cast<int>(config.getDatabaseType())), true);

		auto db = environment->openDatabase(transaction, databaseFileName, "", config);

		fatal(db) << "could not open database " << databaseFileName;

		objectsDatabase.set(db);

		for (auto& index : secondaryIndexes) {
			fatal(db->associate(transaction, index.first->getDatabaseHandle(), index.second,
			     DB_CREATE) == 0, "error could not associate secondary index");
		}

		int ret = 0;

		if ((ret = transaction->commit()) != 0) {
			fatal((db_strerror(ret)));
		}

		lastAssociation.set(secondaryIndexes.size());
	} catch (Exception& e) {
		fatal(e.getMessage());
	}
}

void LocalDatabase::closeDatabase() {
	Locker locker(&Environment::guard);

	try {
		getDatabaseHandle()->close(true);

		info("database closed");
	} catch (Exception &e) {
		error("Error closing database (" + databaseFileName + "):");
		error(e.getMessage());
	} catch (...) {
		error("unreported exception caught while trying to open berkeley DB ");
	}
}


Stream* LocalDatabase::compress(Stream* data) {
	static const int zlibChunkSize = Core::getIntProperty("BerkeleyDB.zlibCopmressChunkSize", CHUNK_SIZE / 4);
	static const int compressionLevel = Core::getIntProperty("BerkeleyDB.zlibCompressionLevel", Z_DEFAULT_COMPRESSION);

	Stream* outputStream  = new Stream();
	std::size_t totalSize = 0;

	try {
		z_stream packet;
		packet.zalloc = Z_NULL;
		packet.zfree = Z_NULL;
		packet.opaque = Z_NULL;
		packet.avail_in = 0;
		packet.next_in = Z_NULL;

		deflateInit(&packet, compressionLevel);

		packet.next_in = (Bytef* )(data->getBuffer());
		packet.avail_in = data->size();

		do {
			outputStream->extendSize(zlibChunkSize);

			packet.next_out = (Bytef* )outputStream->getBuffer() + totalSize;
			packet.avail_out = zlibChunkSize;

			int ret = deflate(&packet, Z_FINISH);

			assert(ret == Z_OK || ret == Z_STREAM_END);

			int wrote = zlibChunkSize - packet.avail_out;

			totalSize += wrote;

		} while (packet.avail_out == 0);

		outputStream->setSize(totalSize);

		deflateEnd(&packet);
	} catch (...) {
		Logger::console.fatal("LocalDatabase::compress");
	}

	//printf("%d uncompressed -> %d compressed\n", data->size(), outputStream->size());

	//free(outputData);

	return outputStream;
}

void LocalDatabase::uncompress(void* data, uint64 size, ObjectInputStream* decompressedData) {
	static const int zlibChunkSize = Core::getIntProperty("BerkeleyDB.zlibChunkSize", CHUNK_SIZE);

	decompressedData->reset();
	std::size_t totalSize = 0;

	try {
		z_stream packet;
		packet.zalloc = Z_NULL;
		packet.zfree = Z_NULL;
		packet.opaque = Z_NULL;
		packet.avail_in = 0;
		packet.next_in = Z_NULL;
		inflateInit(&packet);
		packet.next_in = (Bytef* )(data);
		packet.avail_in = (size);

		do {
			decompressedData->extendSize(zlibChunkSize);

			packet.next_out = (Bytef* )decompressedData->getBuffer() + totalSize;
			packet.avail_out = zlibChunkSize;

			int ret = inflate(&packet, Z_NO_FLUSH);

			if (ret == Z_DATA_ERROR) {
				Logger::console.error("could not decompress stream from database returning uncompressed");

				//decompressedData->writeStream((char*)data, size);

				inflateEnd(&packet);

				return;
			}

			assert(ret == Z_OK || ret == Z_STREAM_END);

			int wrote = zlibChunkSize - packet.avail_out;

			totalSize += wrote;
		} while (packet.avail_out == 0);

		inflateEnd(&packet); //close buffer*/

		decompressedData->setSize(totalSize);

		decompressedData->reset();
	} catch (...) {
		Logger::console.fatal("LocalDatabase::uncompress");
	}

	//free(outputData);
}

int LocalDatabase::getData(Stream* inputKey, ObjectInputStream* objectData, uint32 lockMode, bool compressed) {
	int ret = 0;

	DatabaseEntry key, data;

	key.setData(inputKey->getBuffer(), inputKey->size());

	int i = 0;

	Transaction* transaction = nullptr;

	do {
		ret  = -1;
		TransactionConfig config = TransactionConfig::DEFAULT;

		static const bool mvcc = Core::getIntProperty("BerkeleyDB.MVCC", 0);
		config.setReadUncommitted(!mvcc);
		config.setSnapshot(mvcc);
		config.setNoSync(true);

		transaction = environment->beginTransaction(nullptr, config);

		ret = getDatabaseHandle()->get(transaction, &key, &data, lockMode);

		if (ret == DB_LOCK_DEADLOCK) {
			info() << "deadlock detected in LocalDatabase::get.. retrying iteration " << i;

			transaction->abort();
			transaction = nullptr;
		}

		++i;
	} while (ret == DB_LOCK_DEADLOCK && i < DEADLOCK_MAX_RETRIES);

	if (ret == 0) {
		if (!compression || compressed) {
			objectData->writeStream((const char*) data.getData(), data.getSize());
		} else {
			uncompress(data.getData(), data.getSize(), objectData);
		}

		objectData->reset();
	} else if (ret != DB_NOTFOUND) {
		error("error in LocalDatabase::getData ret " + String(db_strerror(ret)));

		if (transaction != nullptr)
			transaction->abort();

		throw DatabaseException("error in LocalDatabase::getData ret " + String(db_strerror(ret)));
	}

	if (transaction != nullptr) {
		transaction->commit();
	}

	return ret;
}

//stream will be deleted
int LocalDatabase::putData(Stream* inputKey, Stream* stream, engine::db::berkeley::Transaction* masterTransaction) {
	ObjectDatabaseManager* databaseManager = ObjectDatabaseManager::instance();

	CurrentTransaction* transaction = databaseManager->getCurrentTransaction();

	uint64 currentSize = transaction->addUpdateObject(inputKey, stream, this, nullptr);

	if (currentSize > DatabaseManager::MAX_CACHE_SIZE) {
		ObjectDatabaseManager::instance()->commitLocalTransaction(masterTransaction);
	}

	return 0;
}

int LocalDatabase::deleteData(Stream* inputKey, engine::db::berkeley::Transaction* masterTransaction) {
	ObjectDatabaseManager* databaseManager = ObjectDatabaseManager::instance();

	CurrentTransaction* transaction = databaseManager->getCurrentTransaction();

	uint64 currentSize = transaction->addDeleteObject(inputKey, this);

	/*StringBuffer msg;
		msg << "added to deleteData objid" << hex << objKey;
		info(msg);*/

	if (currentSize > DatabaseManager::MAX_CACHE_SIZE) {
		ObjectDatabaseManager::instance()->commitLocalTransaction(masterTransaction);
	}

	return 0;
}

void LocalDatabase::compressDatabaseEntries(engine::db::berkeley::Transaction* transaction) {
	if (compression)
		return;

	HashSet<uint64> compressed;

	LocalDatabaseIterator iterator(transaction, this);

	ObjectInputStream keyStream;
	ObjectInputStream data;

	while (iterator.getNextKeyAndValue(&keyStream, &data)) {
		compression = true;

		ObjectOutputStream* key = new ObjectOutputStream();
		keyStream.copy(key);
		key->reset();

		ObjectOutputStream* dataNew = new ObjectOutputStream();
		data.copy(dataNew);
		dataNew->reset();

		putData(key, dataNew, transaction);
		//assert(iterator.putCurrent(dataNew) == 0);

		compression = false;

		keyStream.clear();
		data.clear();
	}

	compression = true;
}

int LocalDatabase::tryPutData(Stream* inputKey, Stream* stream, engine::db::berkeley::Transaction* transaction) {
	//Locker locker(&Environment::guard);

	int ret = -1;

	DatabaseEntry key, data;
	key.setData(inputKey->getBuffer(), inputKey->size());

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

int LocalDatabase::tryDeleteData(Stream* inputKey, engine::db::berkeley::Transaction* transaction) {
	int ret = -1;

	DatabaseEntry key;
	key.setData(inputKey->getBuffer(), inputKey->size());

	ret = getDatabaseHandle()->del(transaction, &key);

	return ret;
}

void LocalDatabase::reloadParentAssociation() {
	if (parentDatabase) {
		parentDatabase->getDatabaseHandle();
	}
}

void LocalDatabase::associate(LocalDatabase* secondaryDB, int (*callback)(DB *secondary,
			    const DBT *key, const DBT *data, DBT *result)) {
	for (auto& index : secondaryIndexes) {
		if (index.first == secondaryDB) {
			return;
		}
	}

	secondaryDB->setParentDatabase(this);
	secondaryIndexes.add(IndexEntry({secondaryDB, callback}));

	auto val = associations.increment();
	auto db = objectsDatabase.get();

	if (db != nullptr) {
		fatal(db->associate(nullptr, secondaryDB->getDatabaseHandle(), callback,
			     DB_CREATE) == 0, "error could not associate secondary index");

		lastAssociation.set(val);
	}
}

LocalDatabaseIterator::LocalDatabaseIterator(engine::db::berkeley::Transaction* transaction, LocalDatabase* database)
	: Logger("LocalDatabaseIterator") {

	Locker locker(&Environment::guard);

	databaseHandle = database->getDatabaseHandle();

	cursor = databaseHandle->openCursor(transaction);

	localDatabase = database;

	data.setReuseBuffer(true);
	key.setReuseBuffer(true);
}

LocalDatabaseIterator::LocalDatabaseIterator(LocalDatabase* database, const berkeley::CursorConfig& config, bool useCurrentThreadTransaction)
	: Logger("LocalDatabaseIterator") {

	Locker locker(&Environment::guard);

	databaseHandle = database->getDatabaseHandle();

	Transaction* txn = nullptr;//ObjectDatabaseManager::instance()->getLocalTransaction();

/*	if (useCurrentThreadTransaction)
		txn = ObjectDatabaseManager::instance()->getLocalTransaction();*/

	auto cfg = config;

	static bool mvcc = Core::getIntProperty("BerkeleyDB.MVCC", 0);

	if (mvcc && !config.getFlags()) {
		cfg.setSnapshot(true);
	}

	cursor = databaseHandle->openCursor(txn, cfg);

	localDatabase = database;

	data.setReuseBuffer(true);
	key.setReuseBuffer(true);
}

LocalDatabaseIterator::LocalDatabaseIterator(BerkeleyDatabase* dbHandle)
	: Logger("LocalDatabaseIterator") {
	Locker locker(&Environment::guard);

	databaseHandle = dbHandle;
	Transaction* txn = nullptr; // ObjectDatabaseManager::instance()->getLocalTransaction();

	CursorConfig cfg;

	static bool mvcc = Core::getIntProperty("BerkeleyDB.MVCC", 0);

	if (mvcc) {
		cfg.setSnapshot(true);
	}

	cursor = databaseHandle->openCursor(txn, cfg);
	localDatabase = nullptr;

	data.setReuseBuffer(true);
	key.setReuseBuffer(true);
}

LocalDatabaseIterator::~LocalDatabaseIterator() {
	closeCursor();
}

void LocalDatabaseIterator::resetIterator() {
	Locker locker(&Environment::guard);

	if (cursor != nullptr) {
		cursor->close();
		delete cursor;
	}


	Transaction* txn = nullptr;//
	//Transaction* txn = ObjectDatabaseManager::instance()->getLocalTransaction();

	cursor = databaseHandle->openCursor(txn);
}

bool LocalDatabaseIterator::getNextKeyAndValue(ObjectInputStream* key, ObjectInputStream* data, uint32 lockMode, bool compressed) {
	try {
		Locker locker(&Environment::guard);

		if (cursor->getNext(&this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

		key->writeStream((char*)this->key.getData(), this->key.getSize());

		if ((localDatabase && !localDatabase->hasCompressionEnabled()) || compressed)
			data->writeStream((char*)this->data.getData(), this->data.getSize());
		else
			LocalDatabase::uncompress(this->data.getData(), this->data.getSize(), data);

		data->reset();
		key->reset();

	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return true;
}

bool LocalDatabaseIterator::getNextValue(ObjectInputStream* data, uint32 lockMode) {
	try {
		Locker locker(&Environment::guard);

		if (cursor->getNext(&this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

		if ((localDatabase && !localDatabase->hasCompressionEnabled())) {
			data->writeStream((char*)this->data.getData(), this->data.getSize());
		} else {
			LocalDatabase::uncompress(this->data.getData(), this->data.getSize(), data);
		}

		data->reset();

	} catch(Exception &e) {
		error("Error in ObjectDatabaseIterator::getNextValue(String& data)");
		error(e.getMessage());
	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextValue(String& data)");
	}

	return true;
}

bool LocalDatabaseIterator::getNextKey(ObjectInputStream* key, uint32 lockMode) {
	Locker locker(&Environment::guard);

	try {
		if (cursor->getNext(&this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

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

bool LocalDatabaseIterator::getPrevKeyAndValue(ObjectInputStream* key, ObjectInputStream* data, uint32 lockMode) {
	Locker locker(&Environment::guard);

	try {
		if (cursor->getPrev(&this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

		key->writeStream((char*)this->key.getData(), this->key.getSize());

		if (localDatabase && !localDatabase->hasCompressionEnabled())
			data->writeStream((char*)this->data.getData(), this->data.getSize());
		else
			LocalDatabase::uncompress(this->data.getData(), this->data.getSize(), data);

		data->reset();
		key->reset();

	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return true;
}

bool LocalDatabaseIterator::getPrevKey(ObjectInputStream* key, uint32 lockMode) {
	Locker locker(&Environment::guard);

	try {
		if (cursor->getPrev(&this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

		key->writeStream((char*)this->key.getData(), this->key.getSize());
		key->reset();

	} catch(Exception &e) {
		error("Error in ObjectDatabaseIterator::getNextValue(String& data)");
		error(e.getMessage());
	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextValue(String& data)");
	}

	return true;
}

bool LocalDatabaseIterator::getLastKey(ObjectInputStream* key, uint32 lockMode) {
	Locker locker(&Environment::guard);

	try {
		if (cursor->getLast(&this->key, &this->data, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

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


int LocalDatabaseIterator::putCurrent(ObjectOutputStream* data) {
	Locker locker(&Environment::guard);

	DatabaseEntry dataEntry;

	int ret = -1;

	if (localDatabase && !localDatabase->hasCompressionEnabled()) {
		dataEntry.setData(data->getBuffer(), data->size());

		ret = cursor->putCurrent(&dataEntry);

		delete data;
	} else {
		Stream* compressed = LocalDatabase::compress(data);

		delete data;

		dataEntry.setData(compressed->getBuffer(), compressed->size());

		ret = cursor->putCurrent(&dataEntry);

		delete compressed;
	}

	return ret;
}

bool LocalDatabaseIterator::getSearchKey(ObjectOutputStream* key, ObjectInputStream* data, uint32 lockMode) {
	Locker locker(&Environment::guard);

	try {
		DatabaseEntry keyDBE, dataDBE;
		keyDBE.setData(key->getBuffer(), key->size());

		if (cursor->getSearchKey(&keyDBE, &dataDBE, lockMode) != 0) {
			/*this->key.setData(nullptr, 0);
			  this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

/*		if (!localDatabase->hasCompressionEnabled())
			data->writeStream((char*)this->data.getData(), this->data.getSize());
		else
			LocalDatabase::uncompress(this->data.getData(), this->data.getSize(), data);
*/
		data->reset();
		key->reset();

	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return true;
}

bool LocalDatabaseIterator::getSearchKeyRange(ObjectInputStream* key, ObjectInputStream* data, uint32 lockMode) {
	Locker locker(&Environment::guard);

	try {
		this->key.setData(key->getBuffer(), key->size());

		int res = 0;

		if ((res = cursor->getSearchKeyRange(&this->key, &this->data, lockMode))) {
			/*this->key.setData(nullptr, 0);
			  this->data.setData(nullptr, 0);*/
			return false;
		}

		locker.release();

		key->reset();
		key->writeStream((char*)this->key.getData(), this->key.getSize());

		if (localDatabase && !localDatabase->hasCompressionEnabled())
			data->writeStream((char*)this->data.getData(), this->data.getSize());
		else
			LocalDatabase::uncompress(this->data.getData(), this->data.getSize(), data);

		data->reset();
		key->reset();

	} catch (...) {
		error("unreported exception caught in ObjectDatabaseIterator::getNextKeyAndValue(uint64& key, String& data)");
	}

	return true;
}
