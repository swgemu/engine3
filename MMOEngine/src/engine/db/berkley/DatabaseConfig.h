/*
 * DatabaseConfig.h
 *
 *  Created on: 13/02/2010
 *      Author: victor
 */

#ifndef DATABASECONFIG_H_
#define DATABASECONFIG_H_

#include <db.h>

#include "system/platform.h"

namespace engine {
  namespace db {
    namespace berkley {

	class LockMode {
	public:
		const static uint32 DEFAULT = 0;
		const static uint32 READ_COMMITED = DB_READ_COMMITTED;
		const static uint32 READ_UNCOMMITED = DB_READ_UNCOMMITTED;
		const static uint32 RMW = DB_RMW;
		const static uint32 IGNORE_LEASES = DB_IGNORE_LEASE;
	};

	class DatabaseType {
	public:
		const static DBTYPE HASH = DB_HASH;
		const static DBTYPE BTREE = DB_BTREE;
		const static DBTYPE QUEUE = DB_QUEUE;
		const static DBTYPE RECNO = DB_RECNO;
		const static DBTYPE UNKNOWN = DB_UNKNOWN;
	};

	class DatabaseConfig {
		uint32 databaseFlags;
		DBTYPE databaseType;

	public:
		static DatabaseConfig DEFAULT;

	public:
		DatabaseConfig() {
			databaseFlags = 0;
			databaseType = DatabaseType::UNKNOWN;
		}

		inline void setAllowCreate(bool allowCreate) {
			if (!allowCreate)
				databaseFlags &= ~DB_CREATE;
			else
				databaseFlags |= DB_CREATE;
		}

		/**
		 * Enclose the database open within a transaction.
		 */

		void setTransactional(bool transactional) {
			if (transactional)
				databaseFlags |= DB_AUTO_COMMIT;
			else
				databaseFlags &= ~DB_AUTO_COMMIT;
		}

		void setThreaded(bool threaded) {
			if (threaded)
				databaseFlags |= DB_THREAD;
			else
				databaseFlags &= ~DB_THREAD;
		}

		void setReadUncommited(bool value) {
			if (value)
				databaseFlags |= DB_READ_UNCOMMITTED;
			else
				databaseFlags &= ~DB_READ_UNCOMMITTED;
		}

		inline void setType(DBTYPE type) {
			databaseType = type;
		}

		inline uint32 getDatabaseFlags() const {
			return databaseFlags;
		}

		inline DBTYPE getDatabaseType() const {
			return databaseType;
		}

	};

    } // namespace berkley
  } // namespace db
} // namespace engine

#endif /* DATABASECONFIG_H_ */
