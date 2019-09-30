/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * DatabaseConfig.h
 *
 *  Created on: 13/02/2010
 *      Author: victor
 */

#pragma once

#include <db.h>

#include "system/platform.h"

namespace engine {
  namespace db {
    namespace berkeley {

	class LockMode {
	public:
		const static uint32 DEFAULT = 0;
		const static uint32 READ_COMMITED = DB_READ_COMMITTED;
		const static uint32 READ_UNCOMMITED = DB_READ_UNCOMMITTED;
		const static uint32 RMW = DB_RMW;
		const static uint32 IGNORE_LEASES = DB_IGNORE_LEASE;
		const static uint32 MVCC = DB_MULTIVERSION;
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
		bool duplicates;

	public:
		static DatabaseConfig DEFAULT;

	public:
		DatabaseConfig() {
			databaseFlags = 0;
			databaseType = DatabaseType::UNKNOWN;
			duplicates = false;
		}

		void setAllowCreate(bool allowCreate) {
			if (!allowCreate)
				databaseFlags &= ~DB_CREATE;
			else
				databaseFlags |= DB_CREATE;
		}

		void setReadOnly(bool val) {
			if (!val)
				databaseFlags &= ~DB_RDONLY;
			else
				databaseFlags |= DB_RDONLY;
		}

		void setDuplicates(bool val) {
			duplicates = val;
		}

		/**
		 * Enclose the database open within a transaction.
		 */

		void setAutoCommit(bool transactional) {
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

		void setMultiVersionConcurrencyControl(bool value) {
			if (value)
				databaseFlags |= DB_MULTIVERSION;
			else
				databaseFlags &= ~DB_MULTIVERSION;
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

		inline bool hasDuplicates() const {
			return duplicates;
		}

	};

    } // namespace berkeley
  } // namespace db
} // namespace engine

