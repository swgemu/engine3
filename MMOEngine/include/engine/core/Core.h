#ifndef CORE_H_
#define CORE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "engine/stm/TransactionalMemoryManager.h"

#include "engine/db/mysql/MySqlDatabase.h"

#include "TaskManager.h"

#include <new>

namespace engine {
  namespace core {

	class Core : public Thread {
	public:
		Core() {
			initializeContext();
		}

		Core(const char* globallogfile) {
			initializeContext();

			Logger::setGlobalFileLogger(globallogfile);
		}

		virtual void initialize() {
		#ifdef WITH_STM
			engine::stm::Transaction* transaction = engine::stm::Transaction::currentTransaction();

			if (!transaction->commit())
				throw Exception("unable to commit initialization transaction");
		#endif

			TaskManager* taskManager = getTaskManager();
			taskManager->start();
		}

		virtual ~Core() {
			finalizeContext();
		}

		static void scheduleTask(Task* task, uint64 time = 0) {
			TaskManager* taskManager = getTaskManager();
			taskManager->scheduleTask(task, time);
		}

		static void scheduleTask(Task* task, Time& time) {
			TaskManager* taskManager = getTaskManager();
			taskManager->scheduleTask(task, time);
		}

		static TaskManager* getTaskManager() {
			return TaskManager::instance();
		}

	protected:
		void initializeContext() {
			std::set_new_handler(outOfMemoryHandler);

			mysql_library_init(0, NULL, NULL);
			mysql_thread_init();

			Socket::initialize();

			TaskManager* taskManager = getTaskManager();
			taskManager->initialize();

			Thread::initializeThread(this);
		}

		void finalizeContext() {
			TaskManager* taskManager = getTaskManager();
			taskManager->shutdown();

			mysql_thread_end();
			engine::db::mysql::MySqlDatabase::finalizeLibrary();

			NetworkInterface::finalize();

			Logger::closeGlobalFileLogger();
		}

		static void outOfMemoryHandler() {
			System::out << "OutOfMemoryException\n";

			//StackTrace::printStackTrace();

			exit(1);
		}

	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*CORE_H_*/
