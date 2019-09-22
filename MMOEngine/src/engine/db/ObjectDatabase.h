/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef ENGINE_DB_OBJECTDATABASE_H_
#define ENGINE_DB_OBJECTDATABASE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "berkeley/BerkeleyDatabase.h"

#include "LocalDatabase.h"

namespace engine {
  namespace db {

	class DatabaseManager;

	class ObjectDatabase : public LocalDatabase {
	protected:
		ObjectDatabase() {
			dbType = LocalDatabase::OBJECTDATABASE;
		}

	public:
		ObjectDatabase(DatabaseManager* dbEnv, const String& dbFileName, bool compression, DatabaseType databaseType = LocalDatabase::OBJECTDATABASE);

		int getData(uint64 objKey, ObjectInputStream* objectData, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED, bool compressed = false, bool readThreadLocalTransaction = false);
		int getDataNoTx(uint64 objKey, ObjectInputStream* objectData, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED, bool compressed = false);

		//stream will be deleted
		int putData(uint64 objKey, Stream* stream, Object* object, engine::db::berkeley::Transaction* masterTransaction = nullptr);
		int deleteData(uint64 objKey, engine::db::berkeley::Transaction* masterTransaction = nullptr);

		int tryPutData(uint64 objKey, Stream* stream, engine::db::berkeley::Transaction* transaction);
		int tryDeleteData(uint64 objKey, engine::db::berkeley::Transaction* transaction);

		bool isObjectDatabase() const {
			return true;
		}

	};

	class ObjectDatabaseIterator : public LocalDatabaseIterator {
	public:
		ObjectDatabaseIterator(engine::db::berkeley::Transaction* transaction, LocalDatabase* database)
			: LocalDatabaseIterator(transaction, database) {

		}

		ObjectDatabaseIterator(ObjectDatabase* database, const berkeley::CursorConfig& config = berkeley::CursorConfig::DEFAULT, bool useCurrentThreadTransaction = false)
			: LocalDatabaseIterator(database, config, useCurrentThreadTransaction) {
		}

		ObjectDatabaseIterator(engine::db::berkeley::BerkeleyDatabase* databaseHandle) :
			LocalDatabaseIterator(databaseHandle) {

		}

		bool getNextKeyAndValue(uint64& key, ObjectInputStream* data, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED, bool compressed = false);

		//returns raw compressed data
		int getNextKeyAndValueMultiple(berkeley::DatabaseEntry& data, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED);

		bool getNextKey(uint64& key, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED);

		bool getSearchKey(uint64 key, ObjectInputStream* data, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED);
		bool getSearchKeyRange(uint64& key, ObjectInputStream* data, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED);

		bool getLastKey(uint64& key, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED);
		bool getPrevKey(uint64& key, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED);
		bool getPrevKeyAndValue(uint64& key, ObjectInputStream* data, uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED);

	};

  } // namespace db
} // namespace engine

#endif /* ENGINE_DB_OBJECTDATABASE_H_ */
