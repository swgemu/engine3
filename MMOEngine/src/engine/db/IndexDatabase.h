/**
 * @author      : theanswer (theanswer@hyperv)
 * @file        : IndexDatabase
 * @created     : Saturday Aug 03, 2019 08:09:20 UTC
 */

#ifndef INDEXDATABASE_H

#define INDEXDATABASE_H

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "berkeley/BerkeleyDatabase.h"

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
	public:
		IndexDatabaseIterator(engine::db::berkeley::Transaction* transaction, LocalDatabase* database)
			: LocalDatabaseIterator(transaction, database) {
		}

		IndexDatabaseIterator(IndexDatabase* database, const berkeley::CursorConfig& config = berkeley::CursorConfig::DEFAULT, bool useCurrentThreadTransaction = false)
			: LocalDatabaseIterator(database, config, useCurrentThreadTransaction) {
		}

		IndexDatabaseIterator(engine::db::berkeley::BerkeleyDatabase* databaseHandle)
			: LocalDatabaseIterator(databaseHandle) {
		}

		bool setKeyAndGetValue(uint64 key, uint64& primaryKey, ObjectInputStream* data,
				uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED, bool compressed = false);

		bool getNextKeyAndValue(uint64 key, uint64& primaryKey, ObjectInputStream* data,
				uint32 lockMode = berkeley::LockMode::READ_UNCOMMITED, bool compressed = false);
	};

  }
}

#endif /* end of include guard INDEXDATABASE_H */

