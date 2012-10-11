#ifndef ENGINE_DB_DATABASEMANAGER_H_
#define ENGINE_DB_DATABASEMANAGER_H_

#include "system/lang.h"

#include "engine/core/Task.h"

#include "engine/log/Logger.h"

#include "engine/util/Singleton.h"

#include "ObjectDatabase.h"

#include "berkley/Environment.h"

//#define LASTOBJECTIDKEY 0xFFFFFFFFFFFFFFFF;

namespace engine {
  namespace db {

	class DatabaseManager;

	class BerkeleyCheckpointTask : public Task {
		DatabaseManager* manager;

	public:
		BerkeleyCheckpointTask(DatabaseManager* manager) {
			BerkeleyCheckpointTask::manager = manager;
		}

		void run();
	};

	class UpdateObject : public Object {
	public:
		Stream* stream;
		Stream* key;

		engine::db::LocalDatabase* database;

		Object* object;

		//if stream null its a delete action

		UpdateObject() {
			stream = NULL;
			key = NULL;
			database = NULL;
		}

		UpdateObject(Stream* str, Stream* ke, engine::db::LocalDatabase* database, Object* obj) {
			stream = str;
			key = ke;
			this->database = database;
			object = obj;
		}

		UpdateObject(const UpdateObject& i) : Object() {
			stream = i.stream;
			key = i.key;
			this->database = i.database;
			object = i.object;
		}


	};

	class CurrentTransaction  {
		Vector<UpdateObject> updateObjects;
		engine::db::berkley::Environment* databaseEnvironment;


		//stores a references for an object so we dont delete it in the transaction
		Vector<Object* > temporaryObjects;

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

		inline void addUpdateObject(Stream* id, Stream* str, engine::db::LocalDatabase* db, Object* obj) {
			updateObjects.add(UpdateObject(str, id, db, obj));
		}

		inline void addDeleteObject(Stream* id, engine::db::LocalDatabase* db) {
			updateObjects.add(UpdateObject(NULL, id, db, NULL));
		}

		inline Vector<UpdateObject>* getUpdateVector() {
			return &updateObjects;
		}

	};

	class DatabaseManager : public Logger, public Mutex, public Object {
	protected:
		engine::db::berkley::Environment* databaseEnvironment;

		VectorMap<uint16, LocalDatabase*> databases;
		VectorMap<String, uint16> nameDirectory;

		ThreadLocal<CurrentTransaction*> localTransaction;

		LocalDatabase* databaseDirectory;

		uint32 dbEnvironmentFlags;

		uint16 lastTableID;
		uint64 currentVersion;

		Reference<BerkeleyCheckpointTask*> checkpointTask;
		uint32 checkpointTime;

		bool loaded;

	public:
		const static int CHECKPOINTTIME = 1800000; //msec
		const static uint64 LASTOBJECTIDKEY = uint64((uint64)0xFFFFFFFF << 32) + 0xFFFFFFFF;
		const static uint64 VERSIONKEY = uint64((uint64)0xFFFFFFFF << 32) + 0xFFFFFFFE;
		const static uint32 LOCALDATABASE = 1;
		const static uint32 OBJECTDATABASE = 2;

	private:
		void openEnvironment();
		void closeEnvironment();

	protected:
		LocalDatabase* instantiateDatabase(const String& name, bool create, uint16 uniqueID, bool objectDatabase);

	public:
		DatabaseManager();
		~DatabaseManager();

		void loadDatabases(bool truncateDatabases = false);
		void closeDatabases();

		/**
		 * Loads or creates a database in this environment
		 * @param name name of the database
		 * @param create create if no database exists with specified uniqueID/name pair
		 * @param uniqueID if not specified, uniqueID will be set to name.hashCode()
		 */
		LocalDatabase* loadLocalDatabase(const String& name, bool create, uint16 uniqueID = 0xFFFF);

		void checkpoint();

		void getDatabaseName(uint16 tableID, String& name);

		void commitLocalTransaction(engine::db::berkley::Transaction* transaction = NULL);
		void startLocalTransaction();
		void abortLocalTransaction();

		engine::db::berkley::Transaction* startTransaction();
		int commitTransaction(engine::db::berkley::Transaction* transaction);
		/**
		 * Stores a references to an object while the transaction is alive
		 * References will be released when the local transaction is commited
		 */
		void addTemporaryObject(Object* object);

		CurrentTransaction* getCurrentTransaction();

		void updateLastUsedObjectID(uint64 id);
		uint64 getLastUsedObjectID();

		void updateCurrentVersion(uint64 version);

		inline uint64 getCurrentVersion() {
			return currentVersion;
		}

		inline LocalDatabase* getDatabase(uint16 id) {
			//Locker _locker(this);

			return databases.get(id);
		}

		inline LocalDatabase* getDatabase(int idx) {
			//Locker _locker(this);

			return databases.get(idx);
		}

		inline uint16 getDatabaseID(const String& name) {
			//Locker _locker(this);

			return nameDirectory.get(name);
		}

		inline int getTotalDatabaseCount() {
			return databases.size();
		}

		inline engine::db::berkley::Environment* getBerkeleyEnvironment() {
			return databaseEnvironment;
		}

	};

  } // namespace db
} // namespace engine

#endif /* ENGINE_DB_OBJECTDATABASEMANAGER_H_ */
