/*
 * EnvironmentConfig.h
 *
 *  Created on: 13/02/2010
 *      Author: victor
 */

#ifndef ENVIRONMENTCONFIG_H_
#define ENVIRONMENTCONFIG_H_

#include "system/platform.h"
#include <db.h>

namespace engine {
 namespace db {

	 class EnvironmentConfig {
		 uint32 environmentFlags;
		 uint32 threadCount;
		 bool logAutoRemove;
		 int logBufferSize;

	 public:
		 static EnvironmentConfig DEFAULT;

	 public:
		 EnvironmentConfig() {
			 environmentFlags = 0;
			 threadCount = 0;
			 logAutoRemove = false;
			 logBufferSize = 10000; //10mb
		 }


		 inline void setAllowCreate(bool allowCreate) {
			 if (!allowCreate)
				 environmentFlags &= ~DB_CREATE;
			 else
				 environmentFlags |= DB_CREATE;
		 }

		 inline void setInitializeLocking(bool initializeLocking) {
			 if (!initializeLocking)
				 environmentFlags &= ~DB_INIT_LOCK;
			 else
				 environmentFlags |= DB_INIT_LOCK;
		 }

		 inline void setInitializeLogging(bool initializeLogging) {
			 if (!initializeLogging)
				 environmentFlags &= ~DB_INIT_LOG;
			 else
				 environmentFlags |= DB_INIT_LOG;
		 }

		 inline void setLogAutoRemove(bool logAutoRemove) {
			 this->logAutoRemove = logAutoRemove;
		 }

		 inline void setLogBufferSize(int logBufferSize) {
			 this->logBufferSize = logBufferSize;
		 }

		 inline void setThreaded(bool threaded) {
			 if (!threaded)
				 environmentFlags &= ~DB_THREAD;
			 else
				 environmentFlags |= DB_THREAD;
		 }

		 inline void setThreadCount(int count) {
			 threadCount = count;
		 }

		 inline void setTransactional(bool transactional) {
			 if (!transactional)
				 environmentFlags &= ~DB_INIT_TXN;
			 else
				 environmentFlags |= DB_INIT_TXN;
		 }

		 inline void setInitializeCache(bool initializeCache) {
			 if (!initializeCache)
				 environmentFlags &= ~DB_INIT_MPOOL;
			 else
				 environmentFlags |= DB_INIT_MPOOL;
		 }

		 inline uint32 getEnvironmentFlags() const {
			 return environmentFlags;
		 }

		 inline uint32 getThreadCount() const {
			 return threadCount;
		 }

	 };

 }
}

using namespace engine::db;

#endif /* ENVIRONMENTCONFIG_H_ */
