/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef CORE_H_
#define CORE_H_

#include "system/lang.h"

#include "engine/log/Logger.h"

#include "TaskManager.h"

#include "engine/orb/ObjectBroker.h"

#include <new>

namespace engine {
  namespace ORB {
  	  class ObjectBroker;
  	  class DistributedObject;
  }
}

using namespace engine::ORB;

namespace engine {
  namespace core {
	class Core : public Thread {
		static UniqueReference<TaskManager*> taskManager;

		static bool taskManagerShutDown;

		//static ObjectBroker* objectBroker;

	public:
		Core(int logLevel = Logger::INFO);
		Core(const char* globallogfile, int logLevel = Logger::INFO);

		virtual ~Core();

		void start();

		virtual void initialize() = 0;

		void shutdownTaskManager();

		template<class T> static Reference<T*> lookupObject(const String& name);
		static Reference<DistributedObject*> lookupObject(uint64 id);

		/*static void scheduleTask(Task* task, uint64 time = 0);
		static void scheduleTask(Task* task, Time& time);*/

		static TaskManager* getTaskManager();

		static ObjectBroker* getObjectBroker();

	protected:
		void initializeContext(int logLevel);

		void finalizeContext();

		static void outOfMemoryHandler();
	};

	class SegmentationFault : public Exception {
	public:
		SegmentationFault(int cause) : Exception() {
			System::out << "Segmentation Fault caused by " << cause << " at\n";
			printStackTrace();
			exit(cause);
		}

		static int GetSignalNumber() {
			return SIGSEGV;
		}
	};

	template<class T> Reference<T*> Core::lookupObject(const String& name) {
		Reference<DistributedObject*> obj = Core::getObjectBroker()->lookUp(name);

		if (obj == nullptr)
			return nullptr;
		else
			return obj.castTo<T*>();
	}

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*CORE_H_*/
