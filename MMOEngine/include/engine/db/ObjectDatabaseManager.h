#ifndef ENGINE_DB_OBJECTDATABASEMANAGER_H_
#define ENGINE_DB_OBJECTDATABASEMANAGER_H_

#include "system/lang.h"

#include "engine/core/Task.h"

#include "engine/log/Logger.h"

#include "engine/util/Singleton.h"

#include "ObjectDatabase.h"

#include "berkley/Environment.h"

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

	class ObjectDatabaseManager : public Logger, public Singleton<ObjectDatabaseManager>, public Mutex {
		engine::db::berkley::Environment* databaseEnvironment;

		VectorMap<uint16, ObjectDatabase*> databases;
		VectorMap<String, uint16> nameDirectory;

		ObjectDatabase* databaseDirectory;

		uint32 dbEnvironmentFlags;

		uint16 lastTableID;

		Reference<BerkeleyCheckpointTask*> checkpointTask;
		uint32 checkpointTime;

	public:
		const static int CHECKPOINTTIME = 1800000; //msec

	private:
		void openEnvironment();
		void closeEnvironment();

		void loadDatabases();

	public:
		ObjectDatabaseManager();

		~ObjectDatabaseManager();

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
