/*
 * DOBObjectManager.cpp
 *
 *  Created on: 20/08/2009
 *      Author: victor
 */

#ifndef DOBOBJECTMANAGER_CPP_
#define DOBOBJECTMANAGER_CPP_

#include <unistd.h>
#include <chrono>

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

int DOBObjectManager::UPDATETODATABASETIME = 300000;

int MAXOBJECTSTOUPDATEPERTHREAD = 15000;
int INITIALUPDATEMODIFIEDOBJECTSTHREADS = 2;
int MIN_UPDATE_THREADS = 2;
int MAX_UPDATE_THREADS = 8;

DOBObjectManager::DOBObjectManager() : Logger("ObjectManager") {
	nextObjectID = Core::getLongProperty("ObjectManager.initialObjectID", 0x1000000); // reserving first ids for snapshot objects

	Core::initializeProperties("ObjectManager");

	INITIALUPDATEMODIFIEDOBJECTSTHREADS = Core::getIntProperty("ObjectManager.initialUpdateModifiedObjectsThreads", INITIALUPDATEMODIFIEDOBJECTSTHREADS);
	MAXOBJECTSTOUPDATEPERTHREAD = Core::getIntProperty("ObjectManager.maxObjectsToUpdatePerThread", MAXOBJECTSTOUPDATEPERTHREAD);
	MIN_UPDATE_THREADS = Core::getIntProperty("ObjectManager.minUpdateThreads", MIN_UPDATE_THREADS);
	MAX_UPDATE_THREADS = Core::getIntProperty("ObjectManager.maxUpdateThreads", MAX_UPDATE_THREADS);
	UPDATETODATABASETIME = Core::getIntProperty("ObjectManager.updateToDatabaseTime", UPDATETODATABASETIME);

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

	if (servant == nullptr) {
		// object not local we add it to remote DOB <-> hosts map
		remoteDeployedObjects.add(object->_getObjectID(), object);

		return nullptr;
	}

	Locker _locker(this);

	DistributedObjectClassHelper* helper = servant->_getClassHelper();
	DistributedObjectAdapter* adapter = helper->createAdapter(object);

	//printf( "registering adapter for oid %d", (int)object->_getObjectID());

	return localObjectDirectory.add(object->_getObjectID(), adapter);
}

Reference<DistributedObject*> DOBObjectManager::getObject(uint64 objectID) {
	Reference<DistributedObject*> obj = nullptr;

	ReadLocker _locker(this);

	obj = localObjectDirectory.get(objectID);

	if (obj != nullptr)
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
		ObjectOutputStream* objectData = new ObjectOutputStream(8192 / 2, 0);

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

			if (database != nullptr) {
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

	if (table != nullptr) {
		table->deleteData(objectID);

		return 0;
	} else {
		StringBuffer msg;
		msg << "could not delete object id from database table nullptr for id 0x" << hex << objectID;
		error(msg);
	}

	return 1;
}

ObjectDatabase* DOBObjectManager::getTable(uint64 objectID) {
	ObjectDatabase* table = nullptr;
	LocalDatabase* local = nullptr;

	if (objectID != 0) {
		uint16 tableID = (uint16) (objectID >> 48);

		local = databaseManager->getDatabase(tableID);

		if (local == nullptr || !local->isObjectDatabase())
			return nullptr;
		else
			table = static_cast<ObjectDatabase*>(local);
	}

	return table;
}

UpdateModifiedObjectsThread* DOBObjectManager::createUpdateModifiedObjectsThread() {
	int maxCpus = Math::max(1, (int) sysconf(_SC_NPROCESSORS_ONLN));

	UpdateModifiedObjectsThread* thread = new UpdateModifiedObjectsThread(updateModifiedObjectsThreads.size(), this, updateModifiedObjectsThreads.size() % maxCpus);
	thread->start();

	updateModifiedObjectsThreads.add(thread);

	return thread;
}

void DOBObjectManager::updateModifiedObjectsToDatabase() {
	info("starting saving objects to database", true);

	bool rootBroker = DistributedObjectBroker::instance()->isRootBroker();
	//ObjectDatabaseManager::instance()->checkpoint();

	if (objectUpdateInProcess) {
		error("object manager already updating objects to database... try again later");
		return;
	}

	auto startBlock = std::chrono::high_resolution_clock::now();

	Vector<Locker*>* lockers = Core::getTaskManager()->blockTaskManager();

	auto endBlock = std::chrono::high_resolution_clock::now();

	uint64 durationOfBlocking = std::chrono::duration_cast<std::chrono::nanoseconds>(endBlock - startBlock).count();

	info("waited task manager to stop for " + String::valueOf(durationOfBlocking) + " ns", true);

	Locker _locker(this);

	objectUpdateInProcess = true;

	engine::db::berkley::Transaction* transaction = nullptr;

	if (rootBroker) {
		databaseManager->updateLastUsedObjectID(getNextFreeObjectID());

		ObjectDatabaseManager::instance()->commitLocalTransaction();

		transaction = ObjectDatabaseManager::instance()->startTransaction();
	}

	if (updateModifiedObjectsTask->isScheduled())
		updateModifiedObjectsTask->cancel();

	Vector<DistributedObject*> objectsToUpdate;
	Vector<DistributedObject*> objectsToDelete;
	Vector<DistributedObject* >* objectsToDeleteFromRAM = new Vector<DistributedObject* >();

	Timer copy;
	copy.start();

	int numberOfThreads = executeUpdateThreads(&objectsToUpdate, &objectsToDelete,
                                 objectsToDeleteFromRAM, transaction);

	auto copyTime = copy.stopMs();

	info("copied objects into ram in " + String::valueOf(copyTime) + " ms", true);

#ifdef WITH_STM
	TransactionalMemoryManager::instance()->unblockTransactions();
#endif

	onUpdateModifiedObjectsToDatabase();

	Core::getTaskManager()->unblockTaskManager(lockers);
	delete lockers;

	CommitMasterTransactionThread::instance()->startWatch(transaction, &updateModifiedObjectsThreads, updateModifiedObjectsThreads.size(), objectsToDeleteFromRAM);

#ifndef WITH_STM
	_locker.release();
#endif

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

int DOBObjectManager::executeUpdateThreads(Vector<DistributedObject*>* objectsToUpdate, Vector<DistributedObject*>* objectsToDelete,
		Vector<DistributedObject* >* objectsToDeleteFromRAM, engine::db::berkley::Transaction* transaction) {
	totalUpdatedObjects = 0;

	int numberOfThreads = 0;

#ifdef PRINT_OBJECT_COUNT
	VectorMap<String, int> inRamClassCount;
	inRamClassCount.setNullValue(0);

	numberOfThreads = runObjectsMarkedForUpdate(transaction, *objectsToUpdate, *objectsToDelete, *objectsToDeleteFromRAM, &inRamClassCount);
#else
	numberOfThreads = runObjectsMarkedForUpdate(transaction, *objectsToUpdate, *objectsToDelete, *objectsToDeleteFromRAM, nullptr);
#endif

	for (auto thread : updateModifiedObjectsThreads) {
		thread->signalCopyFinished();

		thread->waitFinishedWork();
	}

	return numberOfThreads;

}

void DOBObjectManager::dispatchUpdateModifiedObjectsThread(int& currentThread, int& lastThreadCount,
		int& objectsToUpdateCount, engine::db::berkley::Transaction* transaction,
		Vector<DistributedObject*>& objectsToUpdate, Vector<DistributedObject*>* objectsToDelete) {
	int threadIndex = currentThread++;

	UpdateModifiedObjectsThread* thread = nullptr;

	if (threadIndex >= MAX_UPDATE_THREADS) {
		threadIndex = threadIndex % updateModifiedObjectsThreads.size();
	} else if (updateModifiedObjectsThreads.size() <= threadIndex) {
		createUpdateModifiedObjectsThread();
	}

	thread = updateModifiedObjectsThreads.get(threadIndex);

	thread->waitFinishedWork();

	thread->setObjectsToUpdateVector(&objectsToUpdate);
	thread->setStartOffset(lastThreadCount);
	thread->setEndOffset(lastThreadCount + objectsToUpdateCount);
	thread->setTransaction(transaction);
	thread->setObjectsToDeleteVector(objectsToDelete);

	thread->signalActivity();

	lastThreadCount += objectsToUpdateCount;
	objectsToUpdateCount = 0;
}

int DOBObjectManager::runObjectsMarkedForUpdate(engine::db::berkley::Transaction* transaction,
		Vector<DistributedObject*>& objectsToUpdate, Vector<DistributedObject*>& objectsToDelete,
		Vector<DistributedObject* >& objectsToDeleteFromRAM, VectorMap<String, int>* inRamClassCount) {

	info("starting object map iteration", true);

	objectsToUpdate.removeAll(localObjectDirectory.getObjectHashTable().size(), 1); //need to make sure no reallocs happen or threads will read garbage data
	objectsToDelete.removeAll(100000, 0);

	info("allocated object map to update size", true);

	auto iterator = localObjectDirectory.getObjectHashTable().iterator();
	int objectsToUpdateCount = 0;
	int currentThread = 0;
	int lastThreadCount = 0;

	Timer start;
	start.start();

	while (iterator.hasNext()) {
		DistributedObjectAdapter* adapter = iterator.getNextValue();

		DistributedObject* dobObject = adapter->getStub();

		if (dobObject->getReferenceCount() == 2) // 2 is the lowest count now
			objectsToDeleteFromRAM.emplace(dobObject);
		else if (inRamClassCount != nullptr) {
			String className = TypeInfo<DistributedObject>::getClassName(dobObject, false);

			inRamClassCount->put(className, inRamClassCount->get(className) + 1);
		}

		ManagedObject* managedObject = static_cast<ManagedObject*>(dobObject);

		if (dobObject->_isMarkedForDeletion()) {
			objectsToDelete.emplace(dobObject);
		} else if (dobObject->_isUpdated() && managedObject->isPersistent()) {
			objectsToUpdate.emplace(dobObject);

			objectsToUpdateCount++;
		}

		if (objectsToUpdateCount >= MAXOBJECTSTOUPDATEPERTHREAD) {
			dispatchUpdateModifiedObjectsThread(currentThread, lastThreadCount, objectsToUpdateCount, transaction, objectsToUpdate,
					nullptr);
		}
	}

	if (objectsToUpdateCount || objectsToDelete.size()) {
		dispatchUpdateModifiedObjectsThread(currentThread, lastThreadCount, objectsToUpdateCount, transaction, objectsToUpdate,
				&objectsToDelete);
	}

	auto elapsed = start.stopMs();

	StringBuffer msg;
	msg << "launched " << currentThread << " workers and marked " << objectsToUpdate.size() << " objects to update and "
			<< objectsToDelete.size() << " for deletion in " << elapsed << " ms from " << localObjectDirectory.getObjectHashTable().size() << " total objects";

	info(msg.toString(), true);

	return currentThread;
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
