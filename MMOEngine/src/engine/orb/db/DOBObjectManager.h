/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DOBOBJECTMANAGER_H_
#define DOBOBJECTMANAGER_H_

#include "system/platform.h"

#include "engine/core/Task.h"

#include "engine/log/Logger.h"

#include "engine/db/ObjectDatabase.h"
#include "engine/db/ObjectDatabaseManager.h"

#include "engine/util/ObjectFactory.h"

#include "DistributedObjectDirectory.h"

namespace engine {
	namespace ORB {

	class DistributedObject;
	class DistributedObjectAdapter;
	class DistributedObjectStub;

	class DOBServiceClient;

	class UpdateModifiedObjectsThread;

	class DOBObjectManager : public ReadWriteLock, public Logger {
	protected:
		DistributedObjectDirectory localObjectDirectory;

		AtomicLong nextObjectID;

		ObjectDatabaseManager* databaseManager;

		DistributedObjectMap<HashTable<uint64, DistributedObject*>> remoteDeployedObjects;

		Reference<Task*> updateModifiedObjectsTask;

		Vector<UpdateModifiedObjectsThread*> updateModifiedObjectsThreads;

		bool objectUpdateInProcess;

		AtomicInteger totalUpdatedObjects;

		static int UPDATETODATABASETIME;

	public:
		DOBObjectManager();

		virtual ~DOBObjectManager() {
			//localObjectDirectory.destroyContainingObjects();
		}

		virtual Reference<DistributedObjectStub*> loadPersistentObject(uint64 objid);

		void createBackup();

		static void setUpdateToDatabaseTime(int value) {
			UPDATETODATABASETIME = value;
		}

		void scheduleUpdateToDatabase();

		void cancelUpdateModifiedObjectsTask();

		void updateModifiedObjectsToDatabase();

		virtual void onUpdateModifiedObjectsToDatabase() {
		}

		virtual void onCommitData() {
		}

		int commitUpdatePersistentObjectToDB(DistributedObject* object);
		int commitDestroyObjectToDB(uint64 objectID);

		ObjectDatabase* getTable(uint64 objectID);

		virtual int updatePersistentObject(DistributedObject* object);

		virtual DistributedObjectAdapter* addObject(DistributedObjectStub* object);

		virtual Reference<DistributedObject*> getObject(uint64 objectID);

		virtual DistributedObjectAdapter* getAdapter(uint64 objectID);

		virtual DistributedObjectAdapter* removeObject(uint64 objectID);

		virtual void createObjectID(const String& name, DistributedObjectStub* object);

		virtual uint64 getNextFreeObjectID();

		//virtual void savePersistentObjects();

		DistributedObjectDirectory* getLocalObjectDirectory() {
			return &localObjectDirectory;
		}

	protected:
		void finishObjectUpdate();

		UpdateModifiedObjectsThread* createUpdateModifiedObjectsThread();

		void dispatchUpdateModifiedObjectsThread(int& currentThread, int& lastThreadCount,
				int& objectsToUpdateCount, engine::db::berkley::Transaction* transaction,
				Vector<DistributedObject*>& objectsToUpdate, Vector<DistributedObject*>* objectsToDelete);


		int executeUpdateThreads(Vector<DistributedObject*>* objectsToUpdate, Vector<DistributedObject*>* objectsToDelete,
				Vector<DistributedObject* >* objectsToDeleteFromRAM, engine::db::berkley::Transaction* transaction);

		int runObjectsMarkedForUpdate(engine::db::berkley::Transaction* transaction,
				Vector<DistributedObject*>& objectsToUpdate, Vector<DistributedObject*>& objectsToDelete,
				Vector<DistributedObject* >& objectsToDeleteFromRAM, VectorMap<String, int>* inRamClassCount);



		friend class CommitMasterTransactionThread;
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /* DOBOBJECTMANAGER_H_ */
