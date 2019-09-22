/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * Environment.cpp
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#include "engine/db/DatabaseException.h"

#include "Environment.h"
#include "BTransaction.h"
#include "BerkeleyDatabase.h"
#include "engine/core/Core.h"
#include <thread>

#include <sys/ipc.h>
#include <sys/types.h>

using namespace engine::db::berkeley;

EnvironmentConfig EnvironmentConfig::DEFAULT;
EnvironmentMutex Environment::guard;

namespace BDBNS {
	static Logger logger("BerkeleyEnvironment");

	void thread_id_bdb (DB_ENV *env, pid_t *pid, db_threadid_t *tid) {
		if (pid) {
			*pid = getpid();
		}

		if (tid) {
			*tid = pthread_self();
		}
	}
}

Environment::Environment(const String& directory, const EnvironmentConfig& environmentConfig) {
	Environment::directory = directory;
	Environment::environmentConfig = environmentConfig;

	guard.setEnabled(!environmentConfig.isThreaded());

	int ret = db_env_create(&databaseEnvironment, 0);

	if (ret != 0)
		throw DatabaseException("unable to create environment handle with ret code " + String::valueOf(ret));

	int threadCount = environmentConfig.getThreadCount();

	//System::out << "trying to set " << threadCount << " thread count" << endl;

	//databaseEnvironment->mutex_set_max(databaseEnvironment, 75000);
	databaseEnvironment->set_lk_max_locks(databaseEnvironment, Core::getIntProperty("BerkeleyDB.envMaxLocks", 750000));
	databaseEnvironment->set_lk_max_lockers(databaseEnvironment, Core::getIntProperty("BerkeleyDB.envMaxLockers", 750000));
	databaseEnvironment->set_lk_max_objects(databaseEnvironment, Core::getIntProperty("BerkeleyDB.envMaxLockObjects", 750000));

	databaseEnvironment->set_cachesize(databaseEnvironment, 0, Core::getIntProperty("BerkeleyDB.envCacheSize", 262144000), Core::getIntProperty("BerkeleyDB.envCacheNumber", 0));
	databaseEnvironment->set_lg_bsize(databaseEnvironment, Core::getIntProperty("BerkeleyDB.envLogCache", 262144000));

	databaseEnvironment->set_thread_count(databaseEnvironment, threadCount);

	uint32 envFlags = 0;

	if (environmentConfig.isDirectAccess()) {
		envFlags |= DB_DIRECT_DB;
	}

	if (environmentConfig.isMVCC()) {
		envFlags |= DB_MULTIVERSION;
	}

	if (envFlags) {
		databaseEnvironment->set_flags(databaseEnvironment, envFlags, 1);
	}

	const static int shmKey = Core::getIntProperty("BerkeleyDB.envSHMKey", 0);

	if (shmKey) {
		const static String fileName = Core::getProperty("BerkeleyDB.envSHMKeyFile", "databases/err2.file");
		key_t shm_key = ftok(fileName.toCharArray(), shmKey);

		databaseEnvironment->set_shm_key(databaseEnvironment, shm_key);

		ret = databaseEnvironment->open(databaseEnvironment, directory.toCharArray(), environmentConfig.getEnvironmentFlags() | DB_SYSTEM_MEM, 0);
	} else {
		ret = databaseEnvironment->open(databaseEnvironment, directory.toCharArray(), environmentConfig.getEnvironmentFlags(), 0);
	}

	if (ret != 0)
		throw DatabaseException("unable to open environment handle with ret code " + String(db_strerror(ret)));

	databaseEnvironment->set_isalive(databaseEnvironment, isAlive);
	databaseEnvironment->set_thread_id(databaseEnvironment, BDBNS::thread_id_bdb);

	int lret = databaseEnvironment->set_lk_detect(databaseEnvironment, environmentConfig.getLockDetectMode());

	if (lret != 0) {
		throw DatabaseException("error setting lock detect " + String(db_strerror(lret)));
	}

	databaseEnvironment->set_lg_max(databaseEnvironment, environmentConfig.getMaxLogFileSize());

	// databaseEnvironment->set_timeout(databaseEnvironment, 10000 , DB_SET_LOCK_TIMEOUT);

	if (environmentConfig.getLogAutoRemove())
		databaseEnvironment->log_set_config(databaseEnvironment, DB_LOG_AUTO_REMOVE, 1);

	const static int writesConfig = Core::getIntProperty("BerkeleyDB.envMaxSeqWrites", 0);

	if (writesConfig) {
		int resSleep = databaseEnvironment->set_mp_max_write(databaseEnvironment, writesConfig, Core::getIntProperty("BerkeleyDB.envMaxSeqWriteSleep", 50000));

		if (resSleep != 0) {
			BDBNS::logger.error("could not set max writes");
		}
	}
}

Environment::~Environment() {
	if (databaseEnvironment != nullptr) {
		int ret = close();

		//assert(ret == 0);
		/*if (ret != 0) {
		  throw DatabaseException("unable to open environment handle with ret code " + String::valueOf(ret));
		  }*/

	}
}

EnvironmentMutex::EnvironmentMutex() : enabled(false) {
}

void EnvironmentMutex::lock(bool doLock) {
	if (!enabled)
		return;

	Mutex::lock(doLock);
}

void EnvironmentMutex::lock(Lockable* lockable) {
	if (!enabled)
		return;

	Mutex::lock(lockable);
}

void EnvironmentMutex::unlock(bool doLock) {
	if (!enabled)
		return;

	Mutex::unlock(doLock);
}

int Environment::close() {
	Locker locker(&guard);

	int ret = 0;

	if (databaseEnvironment != nullptr) {
		ret = databaseEnvironment->close(databaseEnvironment, 0);

		databaseEnvironment = nullptr;
	}

	return ret;
}

Transaction* Environment::beginTransaction(Transaction* parent, const TransactionConfig& config) {
	Locker locker(&guard);

	DB_TXN* parentTransaction = nullptr;

	if (parent != nullptr)
		parentTransaction = parent->getDBTXN();

	Transaction* newTransaction = new Transaction();

	int res = databaseEnvironment->txn_begin(databaseEnvironment, parentTransaction, newTransaction->getDBTXNPTR(), config.getFlags() /*| DB_TXN_NOWAIT*/);

	if (res != 0) {
		delete newTransaction;
    		throw DatabaseException("unable to begin transaction with ret code " + String::valueOf(res));
	}

	/*DB_TXN* newTrans = newTransaction->getDBTXN();
	int ret = newTrans->set_timeout(newTrans, 5000, DB_SET_LOCK_TIMEOUT );

	if (ret != 0)
		System::out << String(" error setting timeout " + String(db_strerror(ret)));*/

	return newTransaction;
}

BerkeleyDatabase* Environment::openDatabase(Transaction* txn, const String& fileName, const String& databaseName, const DatabaseConfig& config) {
	Locker locker(&guard);

	BerkeleyDatabase* database = new BerkeleyDatabase(this, txn, fileName, databaseName, config);

	return database;
}

int Environment::detectDeadlocks(uint32 lockDetectMode) {
	Locker locker(&guard);

	return databaseEnvironment->lock_detect(databaseEnvironment, 0, lockDetectMode, nullptr);
}

void Environment::checkpoint(const CheckpointConfig& config) {
	Locker locker(&guard);

	uint32 flags = 0;

	if (config.getForce())
		flags |= DB_FORCE;

	int ret = databaseEnvironment->txn_checkpoint(databaseEnvironment, config.getKBytes(), config.getMinutes(), flags);

	if (ret != 0)
		throw DatabaseException("error while trying checkpoint with ret " + String::valueOf(ret));
}

int Environment::failCheck() {
	Locker locker(&guard);

	return databaseEnvironment->failchk(databaseEnvironment, 0);
}

int Environment::isAlive(DB_ENV* dbenv, pid_t pid, db_threadid_t tid, u_int32_t flags) {
/*	if (pid != Thread::getProcessID())
		return 0;

	return 1; // still running*/

	int alive = 0;

	if (pid == getpid()) {
		alive = 1;
	} else if (kill(pid, 0) == 0) {
		alive = 1;
	} else if (errno == EPERM) {
		alive = 1;
	}

	return alive;
}
