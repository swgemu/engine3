/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * DOBObjectManager.cpp
 *
 *  Created on: 20/08/2009
 *      Author: victor
 */

#ifndef DOBOBJECTMANAGER_CPP_
#define DOBOBJECTMANAGER_CPP_

#include <cstddef>
#include <sstream>
#include <iomanip>

#include "system/platform.h"

#ifndef PLATFORM_WIN
#include <unistd.h>
#endif

#include <chrono>

#include "engine/core/TaskWorkerThread.h"

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

#include "engine/util/flat_hash_map.hpp"

int DOBObjectManager::UPDATETODATABASETIME = 300000;
bool DOBObjectManager::dumpLastModifiedTraces = false;
AtomicBoolean DOBObjectManager::dumpRunning = false;

namespace DOB {
	static int MAXOBJECTSTOUPDATEPERTHREAD = 15000;
	static int INITIALUPDATEMODIFIEDOBJECTSTHREADS = 2;
	static int MIN_UPDATE_THREADS = 2;
	static int MAX_UPDATE_THREADS = 8;

	const ArrayList<void*>& getIgnoreAddresses() {
		const static ArrayList<void*> vec = [] () {
			ArrayList<void*> addressVector;

			const auto addresses = Core::getPropertyVector("ObjectManager.ignoreModifiedTraces");

			for (const auto& address : addresses) {
				if (address.beginsWith("0x")) {
					std::ptrdiff_t val = UnsignedLong::hexvalueOf(address.subString(2));

					addressVector.emplace(reinterpret_cast<void*>(val));
				}
			}

			return addressVector;
		} ();

		return vec;
	}

	bool containsIgnoreAddress(const StackTrace& trace) {
		const auto& addresses = getIgnoreAddresses();

		for (const auto& address : addresses) {
			if (trace.containsAddress(address)) {
				return true;
			}
		}

		return false;
	}
}

DOBObjectManager::DOBObjectManager() : Logger("ObjectManager") {
	nextObjectID = Core::getLongProperty("ObjectManager.initialObjectID", 0x1000000); // reserving first ids for snapshot objects

	Core::initializeProperties("ObjectManager");

	DOB::INITIALUPDATEMODIFIEDOBJECTSTHREADS = Core::getIntProperty("ObjectManager.initialUpdateModifiedObjectsThreads", DOB::INITIALUPDATEMODIFIEDOBJECTSTHREADS);
	DOB::MAXOBJECTSTOUPDATEPERTHREAD = Core::getIntProperty("ObjectManager.maxObjectsToUpdatePerThread", DOB::MAXOBJECTSTOUPDATEPERTHREAD);
	DOB::MIN_UPDATE_THREADS = Core::getIntProperty("ObjectManager.minUpdateThreads", DOB::MIN_UPDATE_THREADS);
	DOB::MAX_UPDATE_THREADS = Core::getIntProperty("ObjectManager.maxUpdateThreads", DOB::MAX_UPDATE_THREADS);
	UPDATETODATABASETIME = Core::getIntProperty("ObjectManager.updateToDatabaseTime", UPDATETODATABASETIME);
	dumpLastModifiedTraces = Core::getIntProperty("ObjectManager.trackLastUpdatedTrace", 0);

	objectUpdateInProgress = false;
	totalUpdatedObjects = 0;
	//commitedObjects.setNoDuplicateInsertPlan();

	updateModifiedObjectsTask = new UpdateModifiedObjectsTask();
	//updateModifiedObjectsTask->schedule(UPDATETODATABASETIME);

	for (int i = 0; i < DOB::INITIALUPDATEMODIFIEDOBJECTSTHREADS; ++i) {
		createUpdateModifiedObjectsThread();
	}

	CommitMasterTransactionThread::instance()->start();
}

void DOBObjectManager::createBackup(int flags) {
	if (DistributedObjectBroker::instance()->isRootBroker())
		ObjectBrokerDirector::instance()->createBackup(flags);
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

int DOBObjectManager::commitUpdatePersistentObjectToDB(DistributedObject* object) {
	totalUpdatedObjects.increment();

	/*if (!((ManagedObject*)object)->isPersistent())
		return 1;*/

	try {
		ManagedObject* managedObject = dynamic_cast<ManagedObject*>(object);

		E3_ASSERT(managedObject);

		ObjectOutputStream* objectData = new ObjectOutputStream(8192 / 2, 0);

		managedObject->writeObject(objectData);

		uint64 oid = object->_getObjectID();
		uint32 lastSaveCRC = managedObject->getLastCRCSave();
		uint32 currentCRC = BaseProtocol::generateCRC(objectData);

		if (lastSaveCRC == currentCRC) {
			static const auto skipSaves = Core::getIntProperty("ObjectManager.dumpModifiedTracesSkipSaves", 0);

			if (dumpLastModifiedTraces && saveCount >= skipSaves) {
				auto trace = managedObject->getLastModifiedTrace();

				if (trace && !DOB::containsIgnoreAddress(*trace)) {
					trace->print();
				}
			}

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
				database->putData(oid, objectData, object);

				managedObject->setLastCRCSave(currentCRC);

				object->_setUpdated(false);

				totalActuallyChangedObjects.increment();

				const static int trackUniqueObjectsSaveDeltas = Core::getIntProperty("ObjectManager.trackUniqueObjectsSaveDeltas", 0);

				if (trackUniqueObjectsSaveDeltas) {
					commitedObjects.put(object);
				}
			} else {
				delete objectData;

				error() << "unknown database id of objectID 0x" << hex << oid;
			}
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
		error() << "could not delete object id from database table nullptr for id 0x" << hex << objectID;
	}

	return 1;
}

ObjectDatabase* DOBObjectManager::getTable(uint64 objectID) const {
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
	int maxCpus = Math::max(1, (int) System::getOnlineProcessors());

	UpdateModifiedObjectsThread* thread = new UpdateModifiedObjectsThread(updateModifiedObjectsThreads.size(),
			this, updateModifiedObjectsThreads.size() % maxCpus);
	thread->start();

	updateModifiedObjectsThreads.add(thread);

	return thread;
}

DOBObjectManager::UpdateCollection DOBObjectManager::collectModifiedObjectsFromThreads(const ArrayList<Pair<Locker*, TaskWorkerThread*>>& lockers, int flags) {
	const static int saveMode = Core::getIntProperty("ObjectManager.saveMode", 0);
	const static int trackUniqueObjectsSaveDeltas = Core::getIntProperty("ObjectManager.trackUniqueObjectsSaveDeltas", 0);

	UpdateCollection collection;

	if (!saveMode) {
		return collection;
	}

	uniqueModifiedObjectValues.clear();
	uniqueDeletedFromDbObjectValues.clear();

	Timer uniqueModsPerf;
	uniqueModsPerf.start();

	const static auto addUniqueObjects = [](auto thread, auto& uniqueMap, auto& uniqueDeleteMap, auto& collection) {
		auto objects = thread->takeModifiedObjects();

		if (objects && trackUniqueObjectsSaveDeltas) {
			for (const auto& val : *objects) {
				uniqueMap.emplace(val);
			}
		}

		auto deleteObjects = thread->takeDeleteFromDatabaseObjects();

		if (deleteObjects && trackUniqueObjectsSaveDeltas) {
			for (const auto& val : *deleteObjects) {
				uniqueDeleteMap.emplace(val);
			}
		}

		if (objects || deleteObjects) {
			collection.emplace(make_pair(objects, deleteObjects));
		}
	};

	auto mainThread = Core::getCoreInstance();

	if (mainThread != nullptr) {
		addUniqueObjects(mainThread, uniqueModifiedObjectValues, uniqueDeletedFromDbObjectValues, collection);
	} else {
		warning("No main core instance found for save event");
	}

	for (const auto& entry : lockers) {
		if (entry.second != nullptr) {
			addUniqueObjects(entry.second, uniqueModifiedObjectValues, uniqueDeletedFromDbObjectValues, collection);
		}
	}

	auto elapsed = uniqueModsPerf.stopMs();

	if (trackUniqueObjectsSaveDeltas) {
		info(true) << "collected " << uniqueModifiedObjectValues.size() <<
			" different modified objects and " << uniqueDeletedFromDbObjectValues.size()
			<< " for db delete from workers in "
			<< elapsed << "ms";
	}

	return collection;
}

void DOBObjectManager::updateModifiedObjectsToDatabase(int flags) {
	info(true) << "starting"
		<< ((flags & SAVE_DELTA) ? " delta" : "")
		<< ((flags & SAVE_FULL) ? " full" : "")
		<< " save of objects to database"
		<< ((flags & SAVE_DEBUG) ? " with debug" : "")
		<< ((flags & SAVE_DUMP) ? " with dump" : "")
		<< ((flags & SAVE_REPORT) ? " with report" : "")
		<< ((flags & SAVE_JSON) ? " with ram json dump" : "");

	const static int saveMode = Core::getIntProperty("ObjectManager.saveMode", 0);
	const static uint32 saveDeltas = Core::getIntProperty("ObjectManager.saveDeltas", 0);

	Vector<DistributedObject*> objectsToUpdate;
	Vector<DistributedObject*> objectsToDelete;

	bool rootBroker = DistributedObjectBroker::instance()->isRootBroker();
	//ObjectDatabaseManager::instance()->checkpoint();

	if (objectUpdateInProgress) {
		error("object manager already updating objects to database... try again later");
		return;
	}

	if (dumpRunning) {
		error() << "dumpRAMtoJSON running, please try again later.";
		return;
	}

	auto objectsToDeleteFromRAM = new Vector<DistributedObject*>();

	Timer stopWaitTimer;
	stopWaitTimer.start();

	UniqueReference<Vector<Pair<Locker*, TaskWorkerThread*>>*> lockers(Core::getTaskManager()->blockTaskManager());

	info(true) << "waited for task manager to stop for " << nsToString(stopWaitTimer.stop());

	Locker _locker(this);

	objectUpdateInProgress = true;

	engine::db::berkeley::Transaction* transaction = nullptr;

	if (rootBroker) {
		databaseManager->updateLastUsedObjectID(getNextFreeObjectID());

		ObjectDatabaseManager::instance()->commitLocalTransaction();

		fatal(!ObjectDatabaseManager::instance()->failCheck(), "database needs recovery");

		transaction = ObjectDatabaseManager::instance()->startTransaction();
	}

	if (updateModifiedObjectsTask->isScheduled())
		updateModifiedObjectsTask->cancel();

	Timer copy;
	copy.start();

	auto collection = collectModifiedObjectsFromThreads(*lockers, flags);

	if (!(flags & SAVE_FULL) && saveMode && (saveDeltaCount++ < saveDeltas)) {
		info("running delta update", true);

		executeDeltaUpdateThreads(collection, transaction, flags);
	} else {
		info("running full update", true);

		executeUpdateThreads(&objectsToUpdate, &objectsToDelete,
                                 objectsToDeleteFromRAM, transaction, flags);

		saveDeltaCount = 0;
	}

#ifdef WITH_STM
	TransactionalMemoryManager::instance()->unblockTransactions();
#endif
	auto copyTime = copy.stopMs();

	Core::getTaskManager()->unblockTaskManager(lockers);

	info(true) << "copied objects into ram in "
	       << copyTime << " ms";

	if (flags & SAVE_FULL && flags & (SAVE_DUMP | SAVE_JSON)) {
		Time startTime;
		StringBuffer buf;
		buf << "log/save-dumps";

		File::mkpath(buf.toString());

		buf << "/dump-" << startTime.getTime();

		String baseFilename = buf.toString();

		if (flags & SAVE_DUMP) {
			dumpSnapshot(baseFilename, startTime, &objectsToUpdate, &objectsToDelete, objectsToDeleteFromRAM, flags);
		}

		if (flags & SAVE_JSON) {
			dumpRAMtoJSON(baseFilename, startTime);
		}
	}

	CommitMasterTransactionThread::instance()->startWatch(transaction, &updateModifiedObjectsThreads,
			updateModifiedObjectsThreads.size(), objectsToDeleteFromRAM);

#ifndef WITH_STM
	_locker.release();
#endif

	onUpdateModifiedObjectsToDatabase(flags); //this might cause some chars to remain dirty in sql until next save, but we dont care

	//cleanup thread objecuts
	for (auto& entry : collection) {
		auto objectsToUpdate = entry.first;
		auto objectsToDelete = entry.second;

		if (objectsToUpdate) {
			for (auto object : *objectsToUpdate) {
				object->release();
			}

			delete objectsToUpdate;
		}

		if (objectsToDelete) {
			for (auto object : *objectsToDelete) {
				object->release();
			}

			delete objectsToDelete;
		}
	}
}

void DOBObjectManager::dumpSnapshot(const String& baseFilename, Time timestamp,
		ArrayList<DistributedObject*>* objectsToUpdate, ArrayList<DistributedObject*>* objectsToDelete,
		ArrayList<DistributedObject* >* objectsToDeleteFromRAM, int flags) {

	StringBuffer details;
	details
		<< " objectsToUpdate = " << commas << objectsToUpdate->size()
		<< "; objectsToDelete = " << objectsToDelete->size()
		<< "; objectsToDeleteFromRAM = " << objectsToDeleteFromRAM->size()
		<< "; flags = " << flags;

	info(true) << "dumpSnapshot:" << details;

	Timer profile;
	profile.start();

	StringBuffer fileName;
	fileName << baseFilename << ".log";
	File dumpFile(fileName.toString());
	FileWriter dumpWriter(&dumpFile, false);

	StringBuffer header;

	header << "# START v1; uptme = " << Logger::getElapsedTime() << "; time = " << timestamp.getMiliTime() << ";" << details << "\n";
	header << "oid\toperation\trefs\tclassName\n";

	dumpWriter << header;

	auto fmtRow = [&](String op, DistributedObject* obj) {
		StringBuffer row;
		row << obj->_getObjectID() << "\t"
			<< op << "\t"
			<< obj->getReferenceCount() << "\t"
			<< TypeInfo<DistributedObject>::getClassName(obj, false)
			<< "\n";

		dumpWriter << row;
	};

	for (auto obj : *objectsToUpdate) {
		fmtRow("Update", obj);
	}

	for (auto obj : *objectsToDelete) {
		fmtRow("Delete", obj);
	}

	for (auto obj : *objectsToDeleteFromRAM) {
		fmtRow("DeleteFromRAM", obj);
	}

	auto elapsedMs = profile.stopMs();

	StringBuffer endLine;
	endLine << "# END elapsedTime = " << msToString(elapsedMs);
	dumpWriter << endLine << "\n";
	dumpWriter.close();

	info(true) << "dumpSnapshot: Dumped to " << fileName << " in " << msToString(elapsedMs);
}

void DOBObjectManager::dumpRAMtoJSON(const String& baseDirname, Time timestamp) {
	if (!dumpRunning.compareAndSet(false, true)) {
		error() << "Previous dump still running, please try again later.";
		return;
	}

	static int numThreads = Core::getIntProperty("ObjectManager.maxDumpThreads", Math::max(1, (int) System::getOnlineProcessors() / 4 * 3));
	int objCount = localObjectDirectory.getObjectHashTable().size();
	int objsPerTask = objCount / numThreads;

	info(true) << "Dumping " << commas << objCount << " RAM objects to " << baseDirname << "/ using " << numThreads << " threads.";

	static TaskQueue* customQueue = []() { return Core::getTaskManager()->initializeCustomQueue("RAMtoJSON", numThreads); }();

	File::mkpath(baseDirname);

	StringBuffer buf;
	buf << baseDirname << "/ram-dump-" << timestamp.getTime();
	String baseFilename = buf.toString();

	auto iterator = localObjectDirectory.getObjectHashTable().iterator();
	int countQueued = 0;
	int taskNumber = 0;
	Vector<uint64> oidsToDump;

	Timer profileQueue;
	Time now, last_rpt;
	auto startNanos = now.getNanoTime();

	profileQueue.start();

	while (iterator.hasNext()) {
		DistributedObjectAdapter* adapter = iterator.getNextValue();

		DistributedObject* dobObject = adapter->getStub();

		ManagedObject* managedObject = static_cast<ManagedObject*>(dobObject);

		if (managedObject != nullptr) {
			oidsToDump.add(managedObject->_getObjectID());
			++countQueued;
		}

		if (oidsToDump.size() >= objsPerTask) {
			dispatchDumpTask("RAMtoJSON", baseFilename, oidsToDump, ++taskNumber);
			oidsToDump.removeRange(0, oidsToDump.size());
		}

		now.updateToCurrentTime();
		int delta = now.getTime() - last_rpt.getTime();

		if (delta > 5) {
			last_rpt.updateToCurrentTime();
			auto elapsedMs = profileQueue.elapsedToNow() / 1000000;
			auto ps = countQueued / (elapsedMs / 1000.0f);
			info(true) << "Queued " << commas << countQueued << " objects (" << ps << "/s) for dump.";
		}
	}

	if (oidsToDump.size() > 0) {
		dispatchDumpTask("RAMtoJSON", baseFilename, oidsToDump, ++taskNumber);
	}

	auto elapsedMs = profileQueue.stopMs();
	auto ps = elapsedMs > 0 ? countQueued / (elapsedMs / 1000.0f) : countQueued;

	info(true)
		<< "Queued " << commas << countQueued
		<< " objects for JSON dump to " << baseFilename << "/"
		<< " in " << msToString(elapsedMs) << " (" << ps << "/s)";

	Core::getTaskManager()->executeTask([baseFilename, countQueued, startNanos]() {
		Logger log;

		log.setLoggingName("RAMtoJSON");
		log.setLogLevel(LogLevel::INFO);
		log.setLogToConsole(true);

		log.info(true) << "Waiting for RAMtoJSON to finish.";
		Core::getTaskManager()->waitForQueueToFinish("RAMtoJSON");

		Time now;
		auto elapsedMs = (now.getNanoTime() - startNanos) / 1000000.0;
		auto ps = elapsedMs > 0 ? countQueued / (elapsedMs / 1000.0f) : countQueued;

		log.info(true)
		    << "RAMtoJSON finished dumping " << commas << countQueued
			<< " objects as JSON to " << baseFilename
			<< " in " << msToString(elapsedMs) << " (" << ps << "/s)";

		DOBObjectManager::dumpRunning.set(false);
	}, "WaitRAMtoJSON");
}

void DOBObjectManager::dispatchDumpTask(const String& queueName, const String& baseFilename, Vector<uint64> oidsToDump, int taskNumber) {
	Core::getTaskManager()->executeTask([baseFilename, oidsToDump, taskId=taskNumber]() {
		int jitter = 25 * taskId;
		Thread::sleep(jitter);

		String taskIdStr = (taskId < 10 ? "0" : "") + String::valueOf(taskId);
		Logger log;
		log.setLoggingName("RAMtoJSON-" + taskIdStr);
		log.setLogLevel(LogLevel::INFO);
		log.setLogToConsole(true);

		String jsonFileName;
		StringBuffer buf;
		buf << baseFilename << "-" << taskIdStr << ".json";
		jsonFileName = buf.toString();

		File jsonDumpFile(jsonFileName);
		FileWriter jsonDumpWriter(&jsonDumpFile, false);

		Time last_rpt;
		int countDumped = 0;
		int countSkipped = 0;
		int countException = 0;
		DOBObjectManager* objectManager = DistributedObjectBroker::instance()->getObjectManager();
		Timer profileExport;

		log.debug() << "Dumping " << commas << oidsToDump.size() << " objects to " << jsonFileName;

		profileExport.start();

		for (int i = 0; i < oidsToDump.size(); ++i) {
			try {
				auto oid = oidsToDump.get(i);

				auto adapter = objectManager->getAdapter(oid);

				if (adapter == nullptr) {
					++countSkipped;
					continue;
				}

				auto dobObject = adapter->getStub();

				if (dobObject == nullptr) {
					++countSkipped;
					continue;
				}

				Reference<ManagedObject*> managedObject = static_cast<ManagedObject*>(dobObject);

				if (managedObject == nullptr) {
					++countSkipped;
					continue;
				}

				ReadLocker lock(managedObject);

				nlohmann::json jsonObject;

				jsonObject["_oid"] = oid;
				jsonObject["_refCount"] = managedObject->getReferenceCount();

				managedObject->writeJSON(jsonObject);

				jsonDumpWriter << jsonObject.dump() << "\n";

				++countDumped;
			} catch(...) {
				++countException;
			}

			int delta = last_rpt.miliDifference();

			if (delta >= 5000 + jitter) {
				last_rpt.updateToCurrentTime();
				auto elapsedMs = profileExport.elapsedToNow() / 1000000;
				auto ps = countDumped / (elapsedMs / 1000.0f);
				auto total = oidsToDump.size();
				int percentComplete = (double)(countDumped) / total * 100;
				log.info(true) << "Dumped " << commas << countDumped << " of " << total << " (" << percentComplete << ") objects (" << ps << "/s)";
			}
		}

		jsonDumpWriter.close();

		auto elapsedMs = profileExport.stopMs();
		auto ps = elapsedMs > 0 ? countDumped / (elapsedMs / 1000.0f) : countDumped;

		log.info(true)
			<< "Dumped " << commas << countDumped
			<< " objects to " << jsonFileName
			<< " in " << msToString(elapsedMs) << " (" << ps << "/s)"
			<< "; skipped " << countSkipped;

		if (countException > 0) {
			log.warning() << "Had " << commas << countException << " exception(s) while dumping objects.";
		}

	}, "DumpJSON", queueName);
}

int DOBObjectManager::executeUpdateThreads(ArrayList<DistributedObject*>* objectsToUpdate, ArrayList<DistributedObject*>* objectsToDelete,
		ArrayList<DistributedObject* >* objectsToDeleteFromRAM, engine::db::berkeley::Transaction* transaction, int flags) {
	totalUpdatedObjects = 0;
	totalActuallyChangedObjects = 0;
	//commitedObjects.removeAll(localObjectDirectory.getSize(), 1000);
	commitedObjects.objects.clear();

	int numberOfThreads = 0;

	const static int reportTopInRam = Core::getIntProperty("ObjectManager.reportTopInRam", 20);
	const static bool alwaysReportTopInRam = Core::getIntProperty("ObjectManager.AlwaysReportTopInRam", 0);

	if (alwaysReportTopInRam) {
		flags |= SAVE_REPORT;
	}

	VectorMap<String, int> inRamClassCount;
	inRamClassCount.setNullValue(0);

	if ((flags & SAVE_REPORT) && reportTopInRam > 0) {
		numberOfThreads = runObjectsMarkedForUpdate(transaction, objectsToUpdate, *objectsToDelete, *objectsToDeleteFromRAM, &inRamClassCount, flags);
	} else {
		numberOfThreads = runObjectsMarkedForUpdate(transaction, objectsToUpdate, *objectsToDelete, *objectsToDeleteFromRAM, nullptr, flags);
	}

	for (auto thread : updateModifiedObjectsThreads) {
		thread->signalCopyFinished();

		thread->waitFinishedWork();
	}

	if (inRamClassCount.size() > 0) {
		VectorMap<int, String> orderedMap(inRamClassCount.size(), 0);
		orderedMap.setAllowDuplicateInsertPlan();

		int max = 0;
		int total = 0;
		for (int i = 0; i < inRamClassCount.size(); ++i) {
			const String& name = inRamClassCount.elementAt(i).getKey();
			int val = inRamClassCount.elementAt(i).getValue();

			if (name.beginsWith("is")) {
				continue;
			}

			if (val > max) {
				max = val;
			}

			total += val;

			orderedMap.put(val, name);
		}

		auto TotalIsPersistent = inRamClassCount.get("isPersistent");
		auto TotalIsNotPersistent = inRamClassCount.get("isNotPersistent");
		int percentPersistent = (double)(TotalIsPersistent) / total * 100;
		int percentNotPersistent = (double)TotalIsNotPersistent / total * 100;

		info(true) << "InRamObjects:" << commas
			<< " total=" << total
			<< "; isPersistent=" << TotalIsPersistent << " (" << percentPersistent << "%)"
			<< "; isNotPersistent=" << TotalIsNotPersistent << " (" << percentNotPersistent << "%)"
			;

		info(true) << "Top " << reportTopInRam << " objects in RAM:";

		auto valWidth = String::withCommas(max).length();

		for (int i = orderedMap.size() - 1, topN = 1;i >= 0 && topN <= reportTopInRam; --i, ++topN) {
			const String& name = orderedMap.elementAt(i).getValue();
			String valCommas = String::withCommas(orderedMap.elementAt(i).getKey());

			std::ostringstream row;
			row << " "
				<< std::setfill(' ')
				<< std::setw(2) << topN << ")  "
				<< std::setw(valWidth) << valCommas.toCharArray() << "  "
				<< name.toCharArray();

			info(true) << row.str();
		}
	}

	return numberOfThreads;
}

int DOBObjectManager::executeDeltaUpdateThreads(UpdateCollection& updateObjects, engine::db::berkeley::Transaction* transaction, int flags) {
	totalUpdatedObjects = 0;
	totalActuallyChangedObjects = 0;
	commitedObjects.objects.clear();

	int currentThread = 0;
	int objectsToUpdateCount = 0;
	int count = 0;

	for (auto& entry : updateObjects) {
		auto updateObjects = entry.first;
		auto deleteObjects = entry.second;

		int lastThreadCount = 0;
		int objectsToUpdateCount = updateObjects ? updateObjects->size() : 0;

		dispatchUpdateModifiedObjectsThread(currentThread, lastThreadCount,
				objectsToUpdateCount, transaction, updateObjects, deleteObjects, flags);

		++count;
	}

	for (auto thread : updateModifiedObjectsThreads) {
		thread->signalCopyFinished();

		Thread::yield();

		thread->waitFinishedWork();
	}

	return count;
}

void DOBObjectManager::dispatchUpdateModifiedObjectsThread(int& currentThread, int& lastThreadCount,
		int& objectsToUpdateCount, engine::db::berkeley::Transaction* transaction,
		ArrayList<DistributedObject*>* objectsToUpdate, ArrayList<DistributedObject*>* objectsToDelete, int flags) {
	int threadIndex = currentThread++;

	UpdateModifiedObjectsThread* thread = nullptr;

	if (threadIndex >= DOB::MAX_UPDATE_THREADS) {
		threadIndex = threadIndex % updateModifiedObjectsThreads.size();
	} else if (updateModifiedObjectsThreads.size() <= threadIndex) {
		createUpdateModifiedObjectsThread();
	}

	thread = updateModifiedObjectsThreads.get(threadIndex);

	thread->waitFinishedWork();

	thread->setObjectsToUpdateVector(objectsToUpdate);
	thread->setStartOffset(lastThreadCount);
	thread->setEndOffset(lastThreadCount + objectsToUpdateCount);
	thread->setTransaction(transaction);
	thread->setObjectsToDeleteVector(objectsToDelete);

	thread->signalActivity();

	Thread::yield();

	lastThreadCount += objectsToUpdateCount;
	objectsToUpdateCount = 0;
}

void DOBObjectManager::SynchronizedCommitedObjects::put(DistributedObject* obj) {
	Locker locker(&mutex);

	objects.emplace(obj);
}

int DOBObjectManager::runObjectsMarkedForUpdate(engine::db::berkeley::Transaction* transaction,
		ArrayList<DistributedObject*>* objectsToUpdate, ArrayList<DistributedObject*>& objectsToDelete,
		ArrayList<DistributedObject* >& objectsToDeleteFromRAM, VectorMap<String, int>* inRamClassCount, int flags) {

	objectsToUpdate->removeAll(localObjectDirectory.getSize(), 1); //need to make sure no reallocs happen or threads will read garbage data
	objectsToDelete.removeAll(100000, 0);

	auto iterator = localObjectDirectory.getObjectHashTable().iterator();
	int objectsToUpdateCount = 0;
	int currentThread = 0;
	int lastThreadCount = 0;

	Timer profile;
	profile.start();

	while (iterator.hasNext()) {
		DistributedObjectAdapter* adapter = iterator.getNextValue();

		DistributedObject* dobObject = adapter->getStub();

		if (dobObject->getReferenceCount() == 2) // 2 is the lowest count now
			objectsToDeleteFromRAM.emplace(dobObject);
		else if (inRamClassCount != nullptr) {
			StringBuffer buf;

			if (dobObject->isPersistent()) {
				inRamClassCount->put("isPersistent", inRamClassCount->get("isPersistent") + 1);
				buf << "Persistent ";
			} else {
				inRamClassCount->put("isNotPersistent", inRamClassCount->get("isNotPersistent") + 1);
				buf << "Non-persistent ";
			}

			std::string className = TypeInfo<DistributedObject>::getClassName(dobObject, false);

			int pos = 0;
			for (int stop = 2; stop > 0 && (pos = className.rfind("::", pos - 1)) != std::string::npos;) {
				if (className[pos] == ':') {
					--stop;
				}
			}

			buf << className.substr(pos + 2);

			auto reportKey = buf.toString();

			inRamClassCount->put(reportKey, inRamClassCount->get(reportKey) + 1);
		}

		ManagedObject* managedObject = static_cast<ManagedObject*>(dobObject);

		if (dobObject->_isMarkedForDeletion()) {
			objectsToDelete.emplace(dobObject);
		} else if (dobObject->_isUpdated() && managedObject->isPersistent()) {
			objectsToUpdate->emplace(dobObject);

			objectsToUpdateCount++;
		}

		if (objectsToUpdateCount >= DOB::MAXOBJECTSTOUPDATEPERTHREAD) {
			dispatchUpdateModifiedObjectsThread(currentThread, lastThreadCount, objectsToUpdateCount, transaction, objectsToUpdate,
					nullptr, flags);
		}
	}

	if (objectsToUpdateCount || objectsToDelete.size()) {
		dispatchUpdateModifiedObjectsThread(currentThread, lastThreadCount, objectsToUpdateCount, transaction, objectsToUpdate,
				&objectsToDelete, flags);
	}

	auto elapsed = profile.stopMs();

	info(true) << "launched " << currentThread << " workers and marked " << commas << objectsToUpdate->size() << " objects to update and "
			<< objectsToDelete.size() << " for deletion in " << msToString(elapsed) << " from " << localObjectDirectory.getObjectHashTable().size() << " total objects";

	return currentThread;
}

void DOBObjectManager::checkCommitedObjects() {
	const static int trackUniqueObjectsSaveDeltas = Core::getIntProperty("ObjectManager.trackUniqueObjectsSaveDeltas", 0);

	if (!trackUniqueObjectsSaveDeltas) {
		return;
	}

	Locker locker(this);

	Timer perf;
	perf.start();

	int i = 0;

	for (auto obj : commitedObjects.objects) {
		++i;

		if (uniqueModifiedObjectValues.find(obj) == uniqueModifiedObjectValues.end()) {
			auto managed = reinterpret_cast<ManagedObject*>(obj);
			//auto impl = static_cast<ManagedObjectImplementation*>(managed->_getImplementationForRead());

			warning() << "missing object in thread local modified objects list 0x"
					<< hex << managed->_getObjectID() << dec << " " << managed->_getName()
					<< " with " << managed->getReferenceCount() << " references left";

			nlohmann::json jsonObject;
			managed->writeJSON(jsonObject);

			warning(jsonObject.dump().c_str());
		}
	}

	auto elapsedMs = perf.stopMs();

	if (i > 0) {
		info(true) << "checked " << i << " commited objects vs modified objects in "
				<< elapsedMs << " ms";
	}
}

void DOBObjectManager::finishObjectUpdate() {
	Locker _locker(this);

	objectUpdateInProgress = false;

	updateModifiedObjectsTask->schedule(UPDATETODATABASETIME);

	info(true) << "marked updated objects: " << commas << totalUpdatedObjects
			<< " commited objects: " << totalActuallyChangedObjects;

	saveCount++;

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
