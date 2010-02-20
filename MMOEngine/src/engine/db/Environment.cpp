/*
 * Environment.cpp
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#include "Environment.h"
#include "Transaction.h"
#include "DatabaseException.h"
#include "BerkeleyDatabase.h"

EnvironmentConfig EnvironmentConfig::DEFAULT;

Environment::Environment(const String& directory, const EnvironmentConfig& environmentConfig) {
	Environment::directory = directory;
	Environment::environmentConfig = environmentConfig;

	int ret = db_env_create(&databaseEnvironment, 0);

	if (ret != 0)
		throw DatabaseException("unable to create environment handle with ret code " + String::valueOf(ret));

	databaseEnvironment->set_thread_count(databaseEnvironment, this->environmentConfig.getThreadCount());

    ret = databaseEnvironment->open(databaseEnvironment, directory.toCharArray(), this->environmentConfig.getEnvironmentFlags(), 0);

    if (ret != 0)
    	throw DatabaseException("unable to open environment handle with ret code " + String::valueOf(ret));

	databaseEnvironment->set_isalive(databaseEnvironment, isAlive);
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

	int res = databaseEnvironment->txn_begin(databaseEnvironment, parentTransaction, newTransaction->getDBTXNPTR(), config.getFlags());

	if (res != 0) {
		delete newTransaction;
    	throw DatabaseException("unable to begin transaction with ret code " + String::valueOf(res));
	}

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
