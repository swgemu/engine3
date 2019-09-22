/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Environment.h
 *
 *  Created on: 13/02/2010
 *      Author: victor
 */

#ifndef ENGINE_DB_BERKLEY_ENVIRONMENT_H_
#define ENGINE_DB_BERKLEY_ENVIRONMENT_H_

#include "system/lang.h"

#include "EnvironmentConfig.h"
#include "TransactionConfig.h"
#include "DatabaseConfig.h"
#include "CheckpointConfig.h"

#include <db.h>

namespace engine {
  namespace db {
    namespace berkeley {

	class Transaction;
	class BerkeleyDatabase;

	class EnvironmentMutex : public Mutex {
		AtomicBoolean enabled;
	public:
		EnvironmentMutex();

		void lock(bool doLock = true) override;
		void lock(Lockable* lockable) override;

		void unlock(bool doLock = true) override;

		void setEnabled(bool val) {
			enabled = val;
		}
	};

	class Environment {
	protected:
		String directory;
		EnvironmentConfig environmentConfig;

		DB_ENV* databaseEnvironment;

	public:
		static EnvironmentMutex guard;

	public:
		Environment(const String& directory, const EnvironmentConfig& environmentConfig = EnvironmentConfig::DEFAULT);
		~Environment();

		/**
		 * Create a new transaction in the database environment.
		 */
		Transaction* beginTransaction(Transaction* parent, const TransactionConfig& config = TransactionConfig::DEFAULT);

		/**
		 *  Open a database.
		 */
		BerkeleyDatabase* openDatabase(Transaction* txn, const String& fileName, const String& databaseName, const DatabaseConfig& config = DatabaseConfig::DEFAULT);

		/**
		 * Run one iteration of the deadlock detector.
		 */
		int detectDeadlocks(uint32 lockDetectMode = LockDetectMode::DEFAULT);

		/**
		 * Synchronously checkpoint the database environment.
		 */
		void checkpoint(const CheckpointConfig& config = CheckpointConfig::DEFAULT);


		/**
		 * Close the database environment, freeing any allocated resources and closing any underlying subsystems.
		 */
		int close();

		int failCheck();

		inline DB_ENV* getDatabaseEnvironmentHandle() const {
			return databaseEnvironment;
		}

		//needs to be implemented, right now works only for current process
		static int isAlive(DB_ENV* dbenv, pid_t pid, db_threadid_t tid, u_int32_t flags);
	};

    } // namespace berkeley
  } // namespace db
} // namespace engine

#endif /* ENGINE_DB_BERKLEY_ENVIRONMENT_H_ */
