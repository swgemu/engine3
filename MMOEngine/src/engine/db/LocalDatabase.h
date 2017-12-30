/*
 * LocalDatabase.h
 *
 *  Created on: 11/11/2010
 *      Author: victor
 */

#ifndef LOCALDATABASE_H_
#define LOCALDATABASE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "berkley/BerkeleyDatabase.h"

namespace engine {
 namespace db {

 class DatabaseManager;

 class LocalDatabase : public Logger {
 protected:
	 engine::db::berkley::BerkeleyDatabase* objectsDatabase;
	 engine::db::berkley::Environment* environment;

	 String databaseFileName;

	 bool compression;

 protected:
	 virtual void closeDatabase();
	 virtual void openDatabase(const engine::db::berkley::DatabaseConfig& dbConfig);
	 virtual void openDatabase();

	 const static int CHUNK_SIZE = 16384;

	 LocalDatabase() {
	 }

 public:
	 const static int DEADLOCK_MAX_RETRIES = 1000;

	 LocalDatabase(engine::db::DatabaseManager* dbEnv, const String& dbFileName, bool compression);
	 virtual ~LocalDatabase();

	 int getData(Stream* inputKey, ObjectInputStream* objectData);

	 //incoming streams will be deleted
	 int putData(Stream* inputKey, Stream* stream, engine::db::berkley::Transaction* masterTransaction = nullptr);
	 int deleteData(Stream* inputKey, engine::db::berkley::Transaction* masterTransaction = nullptr);

	 //incoming streams wont be deleted
	 int tryPutData(Stream* inputKey, Stream* stream, engine::db::berkley::Transaction* transaction);
	 int tryDeleteData(Stream* inputKey, engine::db::berkley::Transaction* transaction);

	 int sync();

	 int truncate();

	 void compressDatabaseEntries(engine::db::berkley::Transaction* transaction);

	 static void uncompress(void* data, uint64 size, ObjectInputStream* decompressedData);
	 static Stream* compress(Stream* data);

	 virtual bool isObjectDatabase() {
		 return false;
	 }

	 inline engine::db::berkley::BerkeleyDatabase* getDatabaseHandle() {
		 return objectsDatabase;
	 }

	 inline void getDatabaseName(String& name) {
		 name = databaseFileName.replaceFirst(".db", "");
	 }

	 inline const String& getDatabaseFileName() const {
		 return databaseFileName;
	 }

	 inline bool hasCompressionEnabled() {
		 return compression;
	 }

 };

 class LocalDatabaseIterator : public Logger {
	 engine::db::berkley::Cursor* cursor;
	 engine::db::berkley::BerkeleyDatabase* databaseHandle;

	 engine::db::berkley::DatabaseEntry key, data;

	 LocalDatabase* localDatabase;

 public:
	 LocalDatabaseIterator(engine::db::berkley::Transaction* transaction, LocalDatabase* database);
	 LocalDatabaseIterator(LocalDatabase* database, bool useCurrentThreadTransaction = false);
	 LocalDatabaseIterator(engine::db::berkley::BerkeleyDatabase* databaseHandle);
	 ~LocalDatabaseIterator();

	 void resetIterator();

	 bool getNextKeyAndValue(ObjectInputStream* keyStream, ObjectInputStream* data);
	 bool getNextValue(ObjectInputStream* data);
	 bool getNextKey(ObjectInputStream* key);

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
