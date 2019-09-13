/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * LocalDatabase.h
 *
 *  Created on: 11/11/2010
 *      Author: victor
 */

#ifndef LOCALDATABASE_H_
#define LOCALDATABASE_H_

#include <functional>
#include <map>

#include "system/lang.h"
#include "system/thread/atomic/AtomicInteger.h"

#include "engine/log/Logger.h"

#include "berkeley/BerkeleyDatabase.h"

namespace engine {
 namespace db {

 class DatabaseManager;

 class LocalDatabase : public Logger {
 public:
	 enum DatabaseType {LOCALDATABASE = 1, OBJECTDATABASE, INDEXDATABASE};

 protected:
	 ThreadLocal<engine::db::berkley::BerkeleyDatabase*> objectsDatabase;
	 LocalDatabase* parentDatabase = nullptr;

	 engine::db::berkley::Environment* environment;

	 String databaseFileName;

	 bool compression;

	 DatabaseType dbType = LOCALDATABASE;

	 class IndexEntry {
	 public:
		LocalDatabase* first;
		int(*second) (DB*, const DBT*, const DBT*, DBT*);
	 };

	 ArrayList<IndexEntry> secondaryIndexes;

	 AtomicInteger associations;
	 ThreadLocal<uint64> lastAssociation;

 protected:
	 virtual void closeDatabase();
	 virtual void openDatabase(const engine::db::berkley::DatabaseConfig& dbConfig);
	 virtual void openDatabase();

	 const static int CHUNK_SIZE = 16384;

	 LocalDatabase() {
	 }

 public:
	 static int DEADLOCK_MAX_RETRIES;

	 LocalDatabase(engine::db::DatabaseManager* dbEnv, const String& dbFileName, bool compression, DatabaseType databaseType = LOCALDATABASE);
	 virtual ~LocalDatabase();

	 int getData(Stream* inputKey, ObjectInputStream* objectData, uint32 lockMode = berkley::LockMode::READ_UNCOMMITED, bool compressed = false);

	 //incoming streams will be deleted
	 int putData(Stream* inputKey, Stream* stream, engine::db::berkley::Transaction* masterTransaction = nullptr);
	 int deleteData(Stream* inputKey, engine::db::berkley::Transaction* masterTransaction = nullptr);

	 //incoming streams wont be deleted
	 int tryPutData(Stream* inputKey, Stream* stream, engine::db::berkley::Transaction* transaction);
	 int tryDeleteData(Stream* inputKey, engine::db::berkley::Transaction* transaction);

	 int sync();

	 int truncate();

	 void compressDatabaseEntries(engine::db::berkley::Transaction* transaction);

	 void associate(LocalDatabase* secondaryDB, int (*callback)(DB *secondary,
				     const DBT *key, const DBT *data, DBT *result));

	 static void uncompress(void* data, uint64 size, ObjectInputStream* decompressedData);
	 static Stream* compress(Stream* data);

	 virtual bool isObjectDatabase() const {
		 return false;
	 }

	 virtual bool isIndexDatabase() const {
	 	return false;
	 }

	 DatabaseType getDatabaseType() {
	 	return dbType;
	 }

	 void setParentDatabase(LocalDatabase* database) {
	 	parentDatabase = database;
	 }

	 void reloadParentAssociation();

	 engine::db::berkley::BerkeleyDatabase* getDatabaseHandle();

	 inline void getDatabaseName(String& name) const {
	 	name = getDatabaseName();
	 }

	 inline String getDatabaseName() const {
		 return databaseFileName.replaceFirst(".db", "");
	 }

	 inline const String& getDatabaseFileName() const {
		 return databaseFileName;
	 }

	 inline bool hasCompressionEnabled() const {
		 return compression;
	 }

 };

 class LocalDatabaseIterator : public Logger {
 protected:
	 engine::db::berkley::Cursor* cursor;
	 engine::db::berkley::BerkeleyDatabase* databaseHandle;

	 engine::db::berkley::DatabaseEntry key, data;

	 LocalDatabase* localDatabase;

 public:
	 LocalDatabaseIterator(engine::db::berkley::Transaction* transaction, LocalDatabase* database);
	 LocalDatabaseIterator(LocalDatabase* database, const berkley::CursorConfig& config = berkley::CursorConfig::DEFAULT, bool useCurrentThreadTransaction = false);
	 LocalDatabaseIterator(engine::db::berkley::BerkeleyDatabase* databaseHandle);
	 ~LocalDatabaseIterator();

	 void resetIterator();

	 bool getNextKeyAndValue(ObjectInputStream* keyStream, ObjectInputStream* data, uint32 lockMode = berkley::LockMode::READ_COMMITED, bool compressed = false);
	 bool getNextValue(ObjectInputStream* data, uint32 lockMode = berkley::LockMode::READ_UNCOMMITED);
	 bool getNextKey(ObjectInputStream* key, uint32 lockMode = berkley::LockMode::READ_UNCOMMITED);

	 bool getPrevKey(ObjectInputStream* key, uint32 lockMode = berkley::LockMode::READ_UNCOMMITED);
	 bool getPrevKeyAndValue(ObjectInputStream* keyStream, ObjectInputStream* data, uint32 lockMode = berkley::LockMode::READ_UNCOMMITED);
	 bool getLastKey(ObjectInputStream* key, uint32 lockMode = berkley::LockMode::READ_UNCOMMITED);

	 bool getSearchKey(ObjectOutputStream* key, ObjectInputStream* data, uint32 lockMode = berkley::LockMode::READ_UNCOMMITED);
	 bool getSearchKeyRange(ObjectInputStream* key, ObjectInputStream* data, uint32 lockMode = berkley::LockMode::READ_UNCOMMITED);

	 //data is freed
	 int putCurrent(ObjectOutputStream* data);

	 inline void closeCursor() {
		 if (cursor != nullptr) {
			 int ret = cursor->close();

			 delete cursor;

			 if (ret != 0)
				 throw DatabaseException("could not close cursor ret: " + String::valueOf(ret));
		 }

		 cursor = nullptr;
	 }
 };

 }
}

#endif /* LOCALDATABASE_H_ */
