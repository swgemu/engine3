/*
 * DOBObjectManager.cpp
 *
 *  Created on: 20/08/2009
 *      Author: victor
 */

#ifndef DOBOBJECTMANAGER_CPP_
#define DOBOBJECTMANAGER_CPP_

#include <unistd.h>

#include "engine/orb/DistributedObjectBroker.h"

#include "engine/service/proto/BaseProtocol.h"

#include "engine/core/ManagedObject.h"

#include "engine/orb/control/ObjectBrokerDirector.h"
#include "engine/orb/control/ObjectBrokerAgent.h"

#include "engine/orb/messages/DOBServiceClient.h"
#include "engine/orb/messages/RemoteObjectBroker.h"
#include "engine/orb/messages/SendObjectDataMessage.h"

#include "UpdateModifiedObjectsTask.h"
#include "UpdateModifiedObjectsThread.h"

#include "CommitMasterTransactionThread.h"

//#define PRINT_OBJECT_COUNT

#define MAX_UPDATE_THREADS 20
#define MIN_UPDATE_THREADS 4
#define INITIAL_OBJECT_ID 0x1000000 // reserving first ids for snapshot objects

DOBObjectManager::DOBObjectManager() : Logger("ObjectManager") {
	nextObjectID = INITIAL_OBJECT_ID;

	objectUpdateInProcess = false;
	totalUpdatedObjects = 0;

	updateModifiedObjectsTask = new UpdateModifiedObjectsTask();
	//updateModifiedObjectsTask->schedule(UPDATETODATABASETIME);

	for (int i = 0; i < INITIALUPDATEMODIFIEDOBJECTSTHREADS; ++i) {
		createUpdateModifiedObjectsThread();
	}

	CommitMasterTransactionThread::instance()->start();
}

void DOBObjectManager::createBackup() {
	if (DistributedObjectBroker::instance()->isRootBroker())
		ObjectBrokerDirector::instance()->createBackup();
	else
		warning("remote backup creation not implemented yet");
}

int DOBObjectManager::updatePersistentObject(DistributedObject* object) {
	/*Packet* msg = new UpdatePersistentObjectMessage(object);
	client->send(msg);

	Packet resp;
	client->read(&resp);

	return UpdatePersistentObjectResponseMessage::parseReturnValue(&resp);*/
	return 0;
}

DistributedObjectAdapter* DOBObjectManager::addObject(DistributedObjectStub* object) {
	DistributedObjectServant* servant = object->getServant();

	if (servant == NULL) {
		// object not local we add it to remote DOB <-> hosts map
		remoteDeployedObjects.add(object->_getObjectID(), object);

		return NULL;
	}

	Locker _locker(this);

	DistributedObjectClassHelper* helper = servant->_getClassHelper();
	DistributedObjectAdapter* adapter = helper->createAdapter(object);

	//printf( "registering adapter for oid %d", (int)object->_getObjectID());

	return localObjectDirectory.add(object->_getObjectID(), adapter);
}

Reference<DistributedObject*> DOBObjectManager::getObject(uint64 objectID) {
	Reference<DistributedObject*> obj = NULL;

	Locker _locker(this);

	obj = localObjectDirectory.get(objectID);

	if (obj != NULL)
		return obj;
	else {
		//get the object from the remote DOB map
		obj = remoteDeployedObjects.get(objectID);
	}

	return obj;
}

DistributedObjectAdapter* DOBObjectManager::getAdapter(uint64 objectID) {
	Locker _locker(this);

	DistributedObjectAdapter* adapter =  localObjectDirectory.getAdapter(objectID);

	return adapter;
}

DistributedObjectAdapter* DOBObjectManager::removeObject(uint64 objectID) {
	Locker _locker(this);

	DistributedObjectAdapter* object = localObjectDirectory.remove(objectID);

	return object;
}

void DOBObjectManager::createObjectID(const String& name, DistributedObjectStub* object) {
	Locker _locker(this);

	uint64 objectid = object->_getObjectID();

	if (objectid == 0) {
		objectid = getNextFreeObjectID();
		object->_setObjectID(objectid);
	}

	//printf("setting new objectid to :%d\n", (int)object->_getObjectID());

	if (name.isEmpty()) {
		/*if (object->_getName().isEmpty())*/ {
			StringBuffer orbname;
			orbname << "_OrbObject" << objectid;

			object->_setName(orbname.toString());
		}
	} else
		object->_setName(name);
}

uint64 DOBObjectManager::getNextFreeObjectID() {
	DistributedObjectBroker* broker = DistributedObjectBroker::instance();

	if (!broker->isRootBroker()) {
		return broker->getNextFreeObjectID();
	}

	uint64 val = nextObjectID.increment();

	return val;
}

/*void DOBObjectManager::savePersistentObjects() {
	Locker locker(this);

	localObjectDirectory.savePersistentObjects();
}*/

int DOBObjectManager::commitUpdatePersistentObjectToDB(DistributedObject* object) {
	totalUpdatedObjects.increment();

	/*if (!((ManagedObject*)object)->isPersistent())
		return 1;*/

	try {
		ManagedObject* managedObject = static_cast<ManagedObject*>(object);
		ObjectOutputStream* objectData = new ObjectOutputStream(8000);

		managedObject->writeObject(objectData);

		uint64 oid = object->_getObjectID();

		uint32 lastSaveCRC = managedObject->getLastCRCSave();

		uint32 currentCRC = BaseProtocol::generateCRC(objectData);

		if (lastSaveCRC == currentCRC) {
			object->_setUpdated(false);

			delete objectData;
			return 1;
		}

		DistributedObjectBroker* broker = DistributedObjectBroker::instance();

		if (!broker->isRootBroker()) {
			SendObjectDataMessage message(oid, objectData);

			RemoteObjectBroker* remote = dynamic_cast<RemoteObjectBroker*>(broker->getRootBroker());
			remote->getBrokerClient()->sendAndAcceptReply(&message);

			delete objectData;

			return 1;
		} else {
			ObjectDatabase* database = getTable(oid);

			if (database != NULL) {
				//StringBuffer msg;
				/*String dbName;

				database->getDatabaseName(dbName);

				msg << "saving to database with table " << dbName << " and object id 0x" << hex << oid;
					info(msg.toString());*/

				database->putData(oid, objectData, object);

				managedObject->setLastCRCSave(currentCRC);

				object->_setUpdated(false);
			} else {
				delete objectData;

				StringBuffer err;
				err << "unknown database id of objectID 0x" << hex << oid;
				error(err.toString());
			}

			/*objectData.escapeString();

				StringBuffer query;
				query << "UPDATE objects SET data = '" << objectData << "' WHERE objectid = " << object->_getObjectID() << ";";
				ServerDatabase::instance()->executeStatement(query);*/
		}
	} catch (...) {
		error("unreported exception caught in ObjectManager::updateToDatabase(SceneObject* object)");

		throw;
	}

	return 0;
}

int DOBObjectManager::commitDestroyObjectToDB(uint64 objectID) {
	if (!DistributedObjectBroker::instance()->isRootBroker())
		return 1;

	ObjectDatabase* table = getTable(objectID);

	if (table != NULL) {
		table->deleteData(objectID);

		return 0;
	} else {
		StringBuffer msg;
		msg << "could not delete object id from database table NULL for id 0x" << hex << objectID;
		error(msg);
	}

	return 1;
}

ObjectDatabase* DOBObjectManager::getTable(uint64 objectID) {
	ObjectDatabase* table = NULL;
	LocalDatabase* local = NULL;

	if (objectID != 0) {
		uint16 tableID = (uint16) (objectID >> 48);

		local = databaseManager->getDatabase(tableID);

		if (local == NULL || !local->isObjectDatabase())
			return NULL;
		else
			table = static_cast<ObjectDatabase*>(local);
	}

	return table;
}

void DOBObjectManager::updateModifiedObjectsToDatabase() {
	bool rootBroker = DistributedObjectBroker::instance()->isRootBroker();
	//ObjectDatabaseManager::instance()->checkpoint();

	if (objectUpdateInProcess) {
		error("object manager already updating objects to database... try again later");
		return;
	}

	Vector<Locker*>* lockers = Core::getTaskManager()->blockTaskManager();

	Locker _locker(this);

	objectUpdateInProcess = true;

	engine::db::berkley::Transaction* transaction = NULL;

	if (rootBroker) {
		databaseManager->updateLastUsedObjectID(getNextFreeObjectID());

		ObjectDatabaseManager::instance()->commitLocalTransaction();

		transaction = ObjectDatabaseManager::instance()->startTransaction();
	}

	if (updateModifiedObjectsTask->isScheduled())
		updateModifiedObjectsTask->cancel();

	info("starting saving objects to database", true);

	Vector<DistributedObject*> objectsToUpdate;
	Vector<DistributedObject*> objectsToDelete;
	Vector<DistributedObject* >* objectsToDeleteFromRAM = new Vector<DistributedObject* >();

#ifdef PRINT_OBJECT_COUNT
	VectorMap<String, int> inRamClassCount;
	inRamClassCount.setNullValue(0);

	localObjectDirectory.getObjectsMarkedForUpdate(objectsToUpdate, objectsToDelete, *objectsToDeleteFromRAM, &inRamClassCount);
#else
	localObjectDirectory.getObjectsMarkedForUpdate(objectsToUpdate, objectsToDelete, *objectsToDeleteFromRAM, NULL);
#endif

	Time start;

//#ifndef WITH_STM
	int numberOfThreads = deployUpdateThreads(&objectsToUpdate, &objectsToDelete, transaction);
//#endif

	info("copied objects into ram in " + String::valueOf(start.miliDifference()) + " ms", true);
/*
	info("objects to delete from ram: " + String::valueOf(objectsToDeleteFromRAM->size()), true);

	for (int i = 0; i < objectsToDeleteFromRAM->size(); ++i) {
		DistributedObjectStub* object = static_cast<DistributedObjectStub*>(objectsToDeleteFromRAM->get(i));
		
	localObjectDirectory.removeHelper(object->_getObjectID());
	}

//	objectsToDeleteFromRAM.removeAll();

	info("finished undeploying objects from orb", true);
	*/

#ifdef WITH_STM
	TransactionalMemoryManager::instance()->unblockTransactions();
#endif

	onUpdateModifiedObjectsToDatabase();
	
//#ifndef WITH_STM
	Core::getTaskManager()->unblockTaskManager(lockers);
	delete lockers;
//#endif
/*
	Reference<DestroyFromRamObjectsTask*> dest = new DestroyFromRamObjectsTask(objectsToDeleteFromRAM);
	dest->execute();
*/

	CommitMasterTransactionThread::instance()->startWatch(transaction, &updateModifiedObjectsThreads, numberOfThreads, objectsToDeleteFromRAM);

#ifndef WITH_STM
	_locker.release();
#endif

//	delete objectsToDeleteFromRAM;

#ifdef PRINT_OBJECT_COUNT

	VectorMap<int, String> orderedMap(inRamClassCount.size(), 0);
	orderedMap.setAllowDuplicateInsertPlan();
	
	for (int i = 0; i < inRamClassCount.size(); ++i) {
		const String& name = inRamClassCount.elementAt(i).getKey();
		int val = inRamClassCount.elementAt(i).getValue();
		
		orderedMap.put(val, name);	
	}

	for (int i = 0; i < orderedMap.size(); ++i) {
		const String& name = orderedMap.elementAt(i).getValue();
		int val = orderedMap.elementAt(i).getKey();
		
		printf("%s\t%d\n", name.toCharArray(), val);
	}
#endif

}

UpdateModifiedObjectsThread* DOBObjectManager::createUpdateModifiedObjectsThread() {
	int maxCpus = MAX(1, sysconf(_SC_NPROCESSORS_ONLN));

	UpdateModifiedObjectsThread* thread = new UpdateModifiedObjectsThread(updateModifiedObjectsThreads.size(), this, updateModifiedObjectsThreads.size() % maxCpus);
	thread->start();

	updateModifiedObjectsThreads.add(thread);

	return thread;
}

int DOBObjectManager::deployUpdateThreads(Vector<DistributedObject*>* objectsToUpdate, Vector<DistributedObject*>* objectsToDelete, engine::db::berkley::Transaction* transaction) {
	if (objectsToUpdate->size() == 0)
		return 0;

	totalUpdatedObjects = 0;

	//Time start;

	int numberOfObjects = objectsToUpdate->size();

	info("numberOfObjects to update:" + String::valueOf(numberOfObjects), true);


	int numberOfThreads = numberOfObjects / MAXOBJECTSTOUPDATEPERTHREAD;
	
	numberOfThreads = MIN(numberOfThreads, MAX_UPDATE_THREADS);
	int rest = numberOfThreads > 0 ? numberOfObjects % numberOfThreads : 0;

	if (rest != 0)
		++numberOfThreads;

	while (numberOfThreads > updateModifiedObjectsThreads.size())
		createUpdateModifiedObjectsThread();

	if (numberOfThreads < MIN_UPDATE_THREADS)
		numberOfThreads = MIN_UPDATE_THREADS;

	int numberPerThread  = numberOfObjects / numberOfThreads;

	if (numberPerThread == 0) {
		numberPerThread = numberOfObjects;
		numberOfThreads = 1;
	}

	for (int i = 0; i < numberOfThreads; ++i) {
		UpdateModifiedObjectsThread* thread = updateModifiedObjectsThreads.getUnsafe(i);

		thread->waitFinishedWork();

		int start = i * numberPerThread;
		int end = 0;

		if (i == numberOfThreads - 1) {
			end = numberOfObjects - 1;
			thread->setObjectsToDeleteVector(objectsToDelete);
		} else
			end = start + numberPerThread - 1;

		thread->setObjectsToUpdateVector(objectsToUpdate);
		thread->setStartOffset(start);
		thread->setEndOffset(end);
		thread->setTransaction(transaction);

		thread->signalActivity();
	}

	for (int i = 0; i < numberOfThreads; ++i) {
		UpdateModifiedObjectsThread* thread = updateModifiedObjectsThreads.getUnsafe(i);

		thread->waitFinishedWork();
	}

	return numberOfThreads;

}

void DOBObjectManager::finishObjectUpdate() {
	Locker _locker(this);

	objectUpdateInProcess = false;

	updateModifiedObjectsTask->schedule(UPDATETODATABASETIME);

	info("updated objects: " + String::valueOf(totalUpdatedObjects), true);

	ObjectBrokerAgent::instance()->finishBackup();
}

void DOBObjectManager::scheduleUpdateToDatabase() {
	updateModifiedObjectsTask->schedule(UPDATETODATABASETIME);
}

void DOBObjectManager::cancelUpdateModifiedObjectsTask() {
	Locker locker(this);

	if (updateModifiedObjectsTask->isScheduled())
		updateModifiedObjectsTask->cancel();
}

Reference<DistributedObjectStub*> DOBObjectManager::loadPersistentObject(uint64 objid) {
	Reference<DistributedObjectStub*> obj;

	return obj;
}

#endif /* DOBOBJECTMANAGER_CPP_ */
