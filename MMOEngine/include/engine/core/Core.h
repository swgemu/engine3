#ifndef CORE_H_
#define CORE_H_

#include "system/lang.h"

#include "../log/Logger.h"

#include "TaskManager.h"

#include "../db/MySqlDatabase.h"

#include <new>

namespace engine {
  namespace core {

	class Core {
	public:
		Core() {
			initializeContext();
		}

		Core(const char* globallogfile) {
			initializeContext();

			Logger::setGlobalFileLogger(globallogfile);
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

			Socket::initialize();

			TaskManager* taskManager = getTaskManager();
			taskManager->initialize();
		}

		void finalizeContext() {
			TaskManager* taskManager = getTaskManager();
			taskManager->shutdown();

			MySqlDatabase::finalizeLibrary();

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
