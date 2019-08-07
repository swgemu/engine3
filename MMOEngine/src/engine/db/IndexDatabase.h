/**
 * @author      : theanswer (theanswer@hyperv)
 * @file        : IndexDatabase
 * @created     : Saturday Aug 03, 2019 08:09:20 UTC
 */

#ifndef INDEXDATABASE_H

#define INDEXDATABASE_H

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "berkley/BerkeleyDatabase.h"

#include "ObjectDatabase.h"

namespace engine {
  namespace db {

	class DatabaseManager;

	class IndexDatabase : public ObjectDatabase {
	protected:
		IndexDatabase();

	public:
		IndexDatabase(DatabaseManager* dbEnv, const String& dbFileName, bool compression);

	 	bool isIndexDatabase() const {
			return true;
		}

	};

	class IndexDatabaseIterator : public LocalDatabaseIterator {
	protected:
		berkley::DatabaseEntry searchKey;
		uint64 searchID = 0;

	public:
		IndexDatabaseIterator(engine::db::berkley::Transaction* transaction, LocalDatabase* database)
			: LocalDatabaseIterator(transaction, database) {
		//		searchKey.setReuseBuffer(true);
		}

		IndexDatabaseIterator(IndexDatabase* database, const berkley::CursorConfig& config = berkley::CursorConfig::DEFAULT, bool useCurrentThreadTransaction = false)
			: LocalDatabaseIterator(database, config, useCurrentThreadTransaction) {
		//		searchKey.setReuseBuffer(true);
		}

		IndexDatabaseIterator(engine::db::berkley::BerkeleyDatabase* databaseHandle)
			: LocalDatabaseIterator(databaseHandle) {
		//		searchKey.setReuseBuffer(true);
		}

		bool setKeyAndGetValue(uint64 key, uint64& primaryKey, ObjectInputStream* data,
				uint32 lockMode = berkley::LockMode::READ_UNCOMMITED, bool compressed = false);

		bool getNextKeyAndValue(uint64 key, uint64& primaryKey, ObjectInputStream* data,
				uint32 lockMode = berkley::LockMode::READ_UNCOMMITED, bool compressed = false);
	};

  }
}

#endif /* end of include guard INDEXDATABASE_H */

