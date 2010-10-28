#ifndef ENGINE_DB_OBJECTDATABASEMANAGER_H_
#define ENGINE_DB_OBJECTDATABASEMANAGER_H_

#include "system/lang.h"

#include "engine/core/Task.h"

#include "engine/log/Logger.h"

#include "engine/util/Singleton.h"

#include "ObjectDatabase.h"

#include "berkley/Environment.h"

//#define LASTOBJECTIDKEY 0xFFFFFFFFFFFFFFFF;

namespace engine {
  namespace db {

	class ObjectDatabaseManager;

	class BerkeleyCheckpointTask : public Task {
		ObjectDatabaseManager* manager;

	public:
		BerkeleyCheckpointTask(ObjectDatabaseManager* manager) {
			BerkeleyCheckpointTask::manager = manager;
		}

		void run();
	};

	class UpdateObject : public Object {
	public:
		Stream* stream;
		uint64 objectid;

		engine::db::ObjectDatabase* database;

		Reference<Object*> object;

		//if stream null its a delete action

		UpdateObject() {
			stream = NULL;
			objectid = 0;
			database = NULL;
		}

		UpdateObject(Stream* str, uint64 id, engine::db::ObjectDatabase* database, Object* obj) {
			stream = str;
			objectid = id;
			this->database = database;
			object = obj;
		}

		UpdateObject(const UpdateObject& i) : Object() {
			stream = i.stream;
			objectid = i.objectid;
			this->database = i.database;
			object = i.object;
		}


	};

	class CurrentTransaction  {
		Vector<UpdateObject> updateObjects;
		engine::db::berkley::Environment* databaseEnvironment;


		//stores a references for an object so we dont delete it in the transaction
		Vector<Reference<Object*> > temporaryObjects;

	public:
		CurrentTransaction(engine::db::berkley::Environment* env) {
			databaseEnvironment = env;
		}

		inline void addTemporaryObject(Object* obj) {
			temporaryObjects.add(obj);
		}

		inline void clearTemporaryObjects() {
			temporaryObjects.removeAll();
		}

		inline void addUpdateObject(uint64 id, Stream* str, engine::db::ObjectDatabase* db, Object* obj) {
			updateObjects.add(UpdateObject(str, id, db, obj));
		}

		inline void addDeleteObject(uint64 id, engine::db::ObjectDatabase* db) {
			updateObjects.add(UpdateObject(NULL, id, db, NULL));
		}

		inline Vector<UpdateObject>* getUpdateVector() {
			return &updateObjects;
		}

	};

	class ObjectDatabaseManager : public Logger, public Singleton<ObjectDatabaseManager>, public Mutex {
		engine::db::berkley::Environment* databaseEnvironment;

		VectorMap<uint16, ObjectDatabase*> databases;
		VectorMap<String, uint16> nameDirectory;

		ThreadLocal<CurrentTransaction> localTransaction;

		ObjectDatabase* databaseDirectory;

		uint32 dbEnvironmentFlags;

		uint16 lastTableID;

		Reference<BerkeleyCheckpointTask*> checkpointTask;
		uint32 checkpointTime;

		bool loaded;

	public:
		const static int CHECKPOINTTIME = 1800000; //msec
		const static uint64 LASTOBJECTIDKEY = uint64((uint64)0xFFFFFFFF << 32) + 0xFFFFFFFF;

	private:
		void openEnvironment();
		void closeEnvironment();

	public:
		ObjectDatabaseManager();

		~ObjectDatabaseManager();

		void loadDatabases();
		void closeDatabases();

		/**
		 * Loads or creates a database in this environment
		 * @param name name of the database
		 * @param create create if no database exists with specified uniqueID/name pair
		 * @param uniqueID if not specified, uniqueID will be set to name.hashCode()
		 */
		ObjectDatabase* loadDatabase(const String& name, bool create, uint16 uniqueID = 0xFFFF);

		void checkpoint();

		void getDatabaseName(uint16 tableID, String& name);

		void commitLocalTransaction(engine::db::berkley::Transaction* transaction = NULL);
		void startLocalTransaction();
		void abortLocalTransaction();

		engine::db::berkley::Transaction* startTransaction();
		int commitTransaction(engine::db::berkley::Transaction* transaction);
		/**
		 * Stores a references for to an object while the transaction is alive
		 * References will be released when the local transaction is commited
		 */
		void addTemporaryObject(Object* object);

		CurrentTransaction* getCurrentTransaction();

		void updateLastUsedObjectID(uint64 id);
		uint64 getLastUsedObjectID();

		inline ObjectDatabase* getDatabase(uint16 id) {
			//Locker _locker(this);

			return databases.get(id);
		}

		inline ObjectDatabase* getDatabase(int idx) {
			//Locker _locker(this);

			return databases.get(idx);
		}

		inline uint16 getDatabaseID(const String& name) {
			//Locker _locker(this);

			return nameDirectory.get(name);
		}

		inline int getDatabaseCount() {
			return databases.size();
		}

		inline engine::db::berkley::Environment* getBerkeleyEnvironment() {
			return databaseEnvironment;
		}

	};

  } // namespace db
} // namespace engine

#endif /* ENGINE_DB_OBJECTDATABASEMANAGER_H_ */
