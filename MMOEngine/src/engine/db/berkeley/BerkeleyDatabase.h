/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * BerkeleyDatabase.h
 *
 *  Created on: 13/02/2010
 *      Author: victor
 */

#ifndef BERKELEYDATABASE_H_
#define BERKELEYDATABASE_H_

#include <db.h>

#include "engine/db/Database.h"

#include "DatabaseConfig.h"
#include "BTransaction.h"
#include "DatabaseEntry.h"
#include "Cursor.h"

namespace engine {
  namespace db {
    namespace berkeley {

	class Environment;
	class Cursor;

	class BerkeleyDatabase : public Database {
	protected:
		DatabaseConfig databaseConfig;

		Environment* environment = nullptr;

		String filename, databaseName;

		DB *dbp;           /* DB structure handle */

	protected:
		BerkeleyDatabase(Environment* env, Transaction* txn, const String& filename, const String& databaseName, const DatabaseConfig& databaseConfig = DatabaseConfig::DEFAULT);

	public:
		BerkeleyDatabase(const String& filename, const String& databaseName, const DatabaseConfig& databaseConfig = DatabaseConfig::DEFAULT);
		~BerkeleyDatabase();

		/**
		 * Retrieves the key/data pair with the given key.
		 */
		int get(Transaction* txn, DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT);
		int getMultiple(Transaction* txn, DatabaseEntry* key, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT);
		int pGetMultiple(Transaction* txn, DatabaseEntry* key, DatabaseEntry* primaryKey, DatabaseEntry* data, uint32 lockMode = LockMode::DEFAULT);

		/**
		 * Store the key/data pair into the database.
		 */
		int put(Transaction* txn, DatabaseEntry* key, DatabaseEntry* data);

		/**
		 * Return a cursor into the database.
		 */
		Cursor* openCursor(Transaction* txn, const CursorConfig& config = CursorConfig::DEFAULT);

		/**
		 * Flush any cached information to disk.
		 */
		void sync();

		/**
		 * Empty the database, discarding all records it contains.
		 */
		int truncate(Transaction* txn, bool countRecords);

		/**
		 * Remove key/data pairs from the database.
		 */
		int del(Transaction* txn, DatabaseEntry* key);

		/**
		 * Flush any cached database information to disk and discard the database handle.
		 */
		void close(bool noSync);

		/**
		 * Compact database
		 */
		int compact(Transaction* txn, DB_COMPACT* compactData, uint32 flags);

		int associate(Transaction* txn, BerkeleyDatabase* secondary, int (*callback)(DB *secondary,
					    const DBT *key, const DBT *data, DBT *result), u_int32_t flags);

		/**
		 * Returns number of unique keys, returns 0 if it was never calculated
		 */
		uint64 count(bool forceCalculation = false, Transaction* txn = nullptr);

		inline const String& getDatabaseFile() const {
			return filename;
		}

		inline const String& getDatabaseName() const {
			return databaseName;
		}

		inline DatabaseConfig& getDatabaseConfig() {
			return databaseConfig;
		}

		inline const DatabaseConfig& getDatabaseConfig() const {
			return databaseConfig;
		}

		inline Environment* getEnvironment() const {
			return environment;
		}

		inline DB* getDatabaseHandle() const {
			return dbp;
		}

		friend class Environment;
	};

    } // namespace berkeley
  } // namespace db
} // namespace engine

#endif /* BERKELEYDATABASE_H_ */
