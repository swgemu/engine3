#ifndef CORE_H_
#define CORE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "engine/orb/ObjectBroker.h"

#include "TaskManager.h"

#include <new>

namespace engine {
  namespace core {

	class Core : public Thread {
		static TaskManager* taskManager;

		static ObjectBroker* objectBroker;

	public:
		Core();
		Core(const char* globallogfile);

		virtual ~Core();

		void start();

		virtual void initialize() = 0;

		template<class T> static T* lookupObject(const String& name);

		static void scheduleTask(Task* task, uint64 time = 0);
		static void scheduleTask(Task* task, Time& time);

		static TaskManager* getTaskManager();

		static ObjectBroker* getObjectBroker();

	protected:
		void initializeContext();

		void finalizeContext();

		static void outOfMemoryHandler();
	};

	template<class T> T* Core::lookupObject(const String& name) {
		return dynamic_cast<T*>(Core::getObjectBroker()->lookUp(name));
	}

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*CORE_H_*/
