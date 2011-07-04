/*
 * Environment.cpp
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#include "../DatabaseException.h"

#include "Environment.h"
#include "Transaction.h"
#include "BerkeleyDatabase.h"

using namespace engine::db::berkley;

EnvironmentConfig EnvironmentConfig::DEFAULT;

Environment::Environment(const String& directory, const EnvironmentConfig& environmentConfig) {
	Environment::directory = directory;
	Environment::environmentConfig = environmentConfig;

	int ret = db_env_create(&databaseEnvironment, 0);

	if (ret != 0)
		throw DatabaseException("unable to create environment handle with ret code " + String::valueOf(ret));

	int threadCount = environmentConfig.getThreadCount();

	//System::out << "trying to set " << threadCount << " thread count" << endl;

	databaseEnvironment->mutex_set_max(databaseEnvironment, 75000);
	databaseEnvironment->set_lk_max_locks(databaseEnvironment, 75000);
	databaseEnvironment->set_lk_max_lockers(databaseEnvironment, 75000);
	databaseEnvironment->set_lk_max_objects(databaseEnvironment, 75000);

	databaseEnvironment->set_cachesize(databaseEnvironment, 0, 26214400, 0);
	databaseEnvironment->set_lg_bsize(databaseEnvironment, 26214400);

	databaseEnvironment->set_thread_count(databaseEnvironment, threadCount);

	ret = databaseEnvironment->open(databaseEnvironment, directory.toCharArray(), environmentConfig.getEnvironmentFlags() | DB_RECOVER, 0);

	if (ret != 0)
		throw DatabaseException("unable to open environment handle with ret code " + String(db_strerror(ret)));

    databaseEnvironment->set_isalive(databaseEnvironment, isAlive);

    int lret = databaseEnvironment->set_lk_detect(databaseEnvironment, environmentConfig.getLockDetectMode());

    if (lret != 0) {
    	throw DatabaseException("error setting lock detect " + String(db_strerror(lret)));
    }

    databaseEnvironment->set_lg_max(databaseEnvironment, environmentConfig.getMaxLogFileSize());

   // databaseEnvironment->set_timeout(databaseEnvironment, 10000 , DB_SET_LOCK_TIMEOUT);

    if (environmentConfig.getLogAutoRemove())
    	databaseEnvironment->log_set_config(databaseEnvironment, DB_LOG_AUTO_REMOVE, 1);

}

Environment::~Environment() {
	if (databaseEnvironment != NULL) {
		int ret = close();

		if (ret != 0) {
	    	throw DatabaseException("unable to open environment handle with ret code " + String::valueOf(ret));
		}

	}
}

int Environment::close() {
	int ret = 0;

	if (databaseEnvironment != NULL) {
		ret = databaseEnvironment->close(databaseEnvironment, 0);

		if (ret == 0)
			databaseEnvironment = NULL;
	}

	return ret;
}

Transaction* Environment::beginTransaction(Transaction* parent, const TransactionConfig& config) {
	DB_TXN* parentTransaction = NULL;

	if (parent != NULL)
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
	BerkeleyDatabase* database = new BerkeleyDatabase(this, txn, fileName, databaseName, config);

	return database;
}

int Environment::detectDeadlocks(uint32 lockDetectMode) {
	return databaseEnvironment->lock_detect(databaseEnvironment, 0, lockDetectMode, NULL);
}

void Environment::checkpoint(const CheckpointConfig& config) {
	uint32 flags = 0;

	if (config.getForce())
		flags |= DB_FORCE;

	int ret = databaseEnvironment->txn_checkpoint(databaseEnvironment, config.getKBytes(), config.getMinutes(), flags);

	if (ret != 0)
		throw DatabaseException("error while trying checkpoint with ret " + String::valueOf(ret));
}

int Environment::failCheck() {
	return databaseEnvironment->failchk(databaseEnvironment, 0);
}

int Environment::isAlive(DB_ENV* dbenv, pid_t pid, db_threadid_t tid, u_int32_t flags) {
	if (pid != Thread::getProcessID())
		return 0;

	return 1; // still running
}
