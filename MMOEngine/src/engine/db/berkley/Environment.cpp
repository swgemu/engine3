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

	//databaseEnvironment->mutex_set_max(databaseEnvironment, 75000);
	databaseEnvironment->set_lk_max_locks(databaseEnvironment, 750000);
	databaseEnvironment->set_lk_max_lockers(databaseEnvironment, 750000);
	databaseEnvironment->set_lk_max_objects(databaseEnvironment, 750000);
/*
#ifndef VERSION_PUBLIC
	databaseEnvironment->set_cachesize(databaseEnvironment, 0, 262144000 * 4, 8);
	
	uint32 logCache = 1024 * 1024;
	logCache = 1000 * logCache;
	databaseEnvironment->set_lg_bsize(databaseEnvironment, logCache);

	databaseEnvironment->set_thread_count(databaseEnvironment, threadCount);

        key_t shm_key = ftok("/mnt/ssd/workspaceALPHA/MMOCoreORB/bin/databases/err2.file", 7232);
        	
	databaseEnvironment->set_shm_key(databaseEnvironment, shm_key);

	ret = databaseEnvironment->open(databaseEnvironment, directory.toCharArray(), environmentConfig.getEnvironmentFlags() | DB_RECOVER | DB_SYSTEM_MEM, 0);
#else
*/
        databaseEnvironment->set_cachesize(databaseEnvironment, 0, 262144000, 0);
	
        databaseEnvironment->set_lg_bsize(databaseEnvironment, 262144000);

	databaseEnvironment->set_thread_count(databaseEnvironment, threadCount);

        ret = databaseEnvironment->open(databaseEnvironment, directory.toCharArray(), environmentConfig.getEnvironmentFlags() | DB_RECOVER, 0);
        
//#endif

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
/*
#ifndef VERSION_PUBLIC
    int resSleep = databaseEnvironment->set_mp_max_write(databaseEnvironment, 1, 50000 / 2);

    if (resSleep != 0) {
    	printf("could not set max writes\n");
    }
    
    
        int maxwritep;
    unsigned int maxwrite_sleepp;

    databaseEnvironment->get_mp_max_write(databaseEnvironment, &maxwritep, &maxwrite_sleepp);
    

    printf("maxwritep:%d maxwrite_speepp:%d\n", maxwritep, maxwrite_sleepp); 
#endif  
 */   
  
    

}

Environment::~Environment() {
	if (databaseEnvironment != nullptr) {
		int ret = close();

		assert(ret == 0);
		/*if (ret != 0) {
	    	throw DatabaseException("unable to open environment handle with ret code " + String::valueOf(ret));
		}*/

	}
}

int Environment::close() {
	int ret = 0;

	if (databaseEnvironment != nullptr) {
		ret = databaseEnvironment->close(databaseEnvironment, 0);

		if (ret == 0)
			databaseEnvironment = nullptr;
	}

	return ret;
}

Transaction* Environment::beginTransaction(Transaction* parent, const TransactionConfig& config) {
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
	BerkeleyDatabase* database = new BerkeleyDatabase(this, txn, fileName, databaseName, config);

	return database;
}

int Environment::detectDeadlocks(uint32 lockDetectMode) {
	return databaseEnvironment->lock_detect(databaseEnvironment, 0, lockDetectMode, nullptr);
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
