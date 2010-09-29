/*
 * ObjectDatabaseThread.cpp
 *
 *  Created on: Sep 23, 2010
 *      Author: oru
 */

#include "ObjectDatabaseThread.h"
#include "ObjectDatabaseManager.h"

using namespace engine::db;

ObjectDatabaseThread::ObjectDatabaseThread() {
	transactionQueue.setAllowDuplicateInsertPlan();

	doRun = true;
}

void ObjectDatabaseThread::run() {
	while (doRun) {

	}


}

void ObjectDatabaseThread::flush() {
	Locker lock(&mutex);

	while (transactionQueue.size() > 0) {
		CurrentTransaction* transaction = tranasctionQueue.remove(0);
	}
}
