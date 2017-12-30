#ifndef ENGINE_DB_OBJECTDATABASE_H_
#define ENGINE_DB_OBJECTDATABASE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "berkley/BerkeleyDatabase.h"

#include "LocalDatabase.h"

namespace engine {
  namespace db {

	class DatabaseManager;

	class ObjectDatabase : public LocalDatabase {
	protected:
		ObjectDatabase() {
		}

	public:
		ObjectDatabase(DatabaseManager* dbEnv, const String& dbFileName, bool compression);

		int getData(uint64 objKey, ObjectInputStream* objectData);

		//stream will be deleted
		int putData(uint64 objKey, Stream* stream, Object* object, engine::db::berkley::Transaction* masterTransaction = nullptr);
		int deleteData(uint64 objKey, engine::db::berkley::Transaction* masterTransaction = nullptr);

		int tryPutData(uint64 objKey, Stream* stream, engine::db::berkley::Transaction* transaction);
		int tryDeleteData(uint64 objKey, engine::db::berkley::Transaction* transaction);

		bool isObjectDatabase() {
			return true;
		}

	};

	class ObjectDatabaseIterator : public LocalDatabaseIterator {
	public:
		ObjectDatabaseIterator(engine::db::berkley::Transaction* transaction, LocalDatabase* database)
			: LocalDatabaseIterator(transaction, database) {

		}

		ObjectDatabaseIterator(ObjectDatabase* database, bool useCurrentThreadTransaction = false)
			: LocalDatabaseIterator(database, useCurrentThreadTransaction) {

		}

		ObjectDatabaseIterator(engine::db::berkley::BerkeleyDatabase* databaseHandle) :
			LocalDatabaseIterator(databaseHandle) {

		}

		bool getNextKeyAndValue(uint64& key, ObjectInputStream* data);
		bool getNextKey(uint64& key);

	};

  } // namespace db
} // namespace engine

#endif /* ENGINE_DB_OBJECTDATABASE_H_ */
