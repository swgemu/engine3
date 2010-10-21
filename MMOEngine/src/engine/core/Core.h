#ifndef CORE_H_
#define CORE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "engine/stm/TransactionalMemoryManager.h"

#include "engine/db/mysql/MySqlDatabase.h"

#include "TaskManagerImpl.h"
#include "TransactionalTaskManager.h"

#include <new>

namespace engine {
  namespace core {

	class Core : public Thread {
		static TaskManager* taskManager;

	public:
		Core();
		Core(const char* globallogfile);

		virtual ~Core();

		virtual void initialize();

		static void scheduleTask(Task* task, uint64 time = 0);
		static void scheduleTask(Task* task, Time& time);

		static void commitTask();

		static TaskManager* getTaskManager();

	protected:
		void initializeContext();

		void finalizeContext();

		static void outOfMemoryHandler();
	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*CORE_H_*/
