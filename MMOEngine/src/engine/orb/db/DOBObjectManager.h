/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef DOBOBJECTMANAGER_H_
#define DOBOBJECTMANAGER_H_

#include "engine/util/bytell_hash_map.hpp"

#include "system/platform.h"
#include "system/lang/Pair.h"
#include "system/thread/atomic/AtomicBoolean.h"

#include "engine/core/Task.h"

#include "engine/log/Logger.h"

#include "engine/db/ObjectDatabase.h"
#include "engine/db/ObjectDatabaseManager.h"

#include "engine/util/ObjectFactory.h"
#include "system/util/SynchronizedSortedVector.h"

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

		ObjectDatabaseManager* databaseManager = nullptr;

		DistributedObjectMap<HashTable<uint64, DistributedObject*>> remoteDeployedObjects;

		Reference<Task*> updateModifiedObjectsTask;

		Vector<UpdateModifiedObjectsThread*> updateModifiedObjectsThreads;

		AtomicBoolean objectUpdateInProgress;

		AtomicInteger totalUpdatedObjects;
		AtomicInteger totalActuallyChangedObjects;

		class SynchronizedCommitedObjects {
		public:
			Mutex mutex;
			ska::bytell_hash_set<DistributedObject*> objects;

			void put(DistributedObject* obj);
		};

		SynchronizedCommitedObjects commitedObjects;

		ska::bytell_hash_set<DistributedObject*> uniqueModifiedObjectValues;
		ska::bytell_hash_set<DistributedObject*> uniqueDeletedFromDbObjectValues;

		int saveCount = 0;
		int saveDeltaCount = 0;

		static int UPDATETODATABASETIME;
		static bool dumpLastModifiedTraces;

	public:
		DOBObjectManager();
		virtual ~DOBObjectManager() {
		}

		enum {
			SAVE_FULL   = 1 << 0,
			SAVE_DELTA  = 1 << 1,
			SAVE_DEBUG  = 1 << 2,
			SAVE_REPORT = 1 << 3,
			SAVE_DUMP   = 1 << 4,
			SAVE_JSON   = 1 << 5
		};

		virtual Reference<DistributedObjectStub*> loadPersistentObject(uint64 objid);

		void createBackup(int flags = SAVE_DELTA);

		static void setUpdateToDatabaseTime(int value) {
			UPDATETODATABASETIME = value;
		}

		static void setDumpLastModifiedTraces(bool val) {
			dumpLastModifiedTraces = val;
		}

		static bool getDumpLastModifiedTraces() {
			return dumpLastModifiedTraces;
		}

		void scheduleUpdateToDatabase();

		void cancelUpdateModifiedObjectsTask();

		void updateModifiedObjectsToDatabase(int flags);

		virtual void onUpdateModifiedObjectsToDatabase(int flags) {
		}

		virtual void onCommitData() {
		}

		int commitUpdatePersistentObjectToDB(DistributedObject* object);
		int commitDestroyObjectToDB(uint64 objectID);

		ObjectDatabase* getTable(uint64 objectID) const;

		virtual int updatePersistentObject(DistributedObject* object);

		virtual DistributedObjectAdapter* addObject(DistributedObjectStub* object);

		virtual Reference<DistributedObject*> getObject(uint64 objectID);

		virtual DistributedObjectAdapter* getAdapter(uint64 objectID);

		virtual DistributedObjectAdapter* removeObject(uint64 objectID);

		virtual void createObjectID(const String& name, DistributedObjectStub* object);

		virtual uint64 getNextFreeObjectID();

		DistributedObjectDirectory* getLocalObjectDirectory() {
			return &localObjectDirectory;
		}

	protected:
		using UpdateCollection = ArrayList<Pair<ArrayList<DistributedObject*>*, ArrayList<DistributedObject*>*>>;

		void finishObjectUpdate();
		void checkCommitedObjects();
		UpdateCollection collectModifiedObjectsFromThreads(const ArrayList<Pair<Locker*, TaskWorkerThread*>>& lockers, int flags);

		UpdateModifiedObjectsThread* createUpdateModifiedObjectsThread();

		void dispatchUpdateModifiedObjectsThread(int& currentThread, int& lastThreadCount,
				int& objectsToUpdateCount, engine::db::berkeley::Transaction* transaction,
				ArrayList<DistributedObject*>* objectsToUpdate, ArrayList<DistributedObject*>* objectsToDelete, int flags);

		int executeUpdateThreads(ArrayList<DistributedObject*>* objectsToUpdate, ArrayList<DistributedObject*>* objectsToDelete,
				ArrayList<DistributedObject* >* objectsToDeleteFromRAM, engine::db::berkeley::Transaction* transaction, int flags);

		int executeDeltaUpdateThreads(UpdateCollection& updateObjects, engine::db::berkeley::Transaction* transaction, int flags);

		int runObjectsMarkedForUpdate(engine::db::berkeley::Transaction* transaction,
				ArrayList<DistributedObject*>* objectsToUpdate, ArrayList<DistributedObject*>& objectsToDelete,
				ArrayList<DistributedObject* >& objectsToDeleteFromRAM, VectorMap<String, int>* inRamClassCount, int flags);

		void dumpSnapshot(const String& baseFilename, Time timestamp,
				ArrayList<DistributedObject*>* objectsToUpdate, ArrayList<DistributedObject*>* objectsToDelete,
				ArrayList<DistributedObject* >* objectsToDeleteFromRAM, int flags);

		void dumpRAMtoJSON(const String& baseFilename, Time timestamp);

		friend class CommitMasterTransactionThread;

	private:
		static AtomicBoolean dumpRunning;
		void dispatchDumpTask(const String& queueName, const String& baseDirname, Vector<uint64> oidsToDump, int taskNumber);
	};

  } // namespace ORB
} // namespace engine

using namespace engine::ORB;

#endif /* DOBOBJECTMANAGER_H_ */
