/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ManagedReference.h"

#include "engine/lua/Lua.h"

SynchronizedHashTable<String, String> Core::properties;
UniqueReference<TaskManager*> Core::taskManager;
bool Core::taskManagerShutDown = false;

//SignalTranslator<SegmentationFault> g_objSegmentationFaultTranslator;
//
//
Core::Core(int logLevel) {
	initializeContext(logLevel);
}

Core::Core(const char* globallogfile, int logLevel) {
	initializeContext(logLevel);

	Logger::setGlobalFileLogger(globallogfile);
}

Core::~Core() {
	finalizeContext();
}

void Core::initializeContext(int logLevel) {
	taskManager = nullptr;

	std::set_new_handler(outOfMemoryHandler);

	if (threadInitializer)
		threadInitializer->onThreadStart(this);

	Thread::initializeThread(this);

	Socket::initialize();

	TaskManager* taskManager = getTaskManager();
	taskManager->setLogLevel(logLevel);
	taskManager->initialize();

	taskManager->start();
}

void Core::parsePropertyData(const String& className, const char* name, LuaObject& table) {
	Core::properties.put((className + ".") + name, Lua::getStringParameter(table.getLuaState()));
}

void Core::initializeProperties(const String& className) {
	try {
		Lua lua;
		lua.init();

		if (!lua.runFile("engine3.lua")) {
			return;
		}

		auto obj = lua.getGlobalObject(className);
		auto L = obj.getLuaState();

		if (obj.isValidTable()) {
			lua_pushnil(L);

			while (lua_next(L, -2) != 0) {
				int type = lua_type(L, -2);

				if (type == LUA_TSTRING) {
					size_t len = 0;
					const char* varName = lua_tolstring(L, -2, &len);

					parsePropertyData(className, varName, obj);
				} else {
					lua_pop(L, 1);
				}
			}

			obj.pop();
		}
	} catch (...) {
	}
}

void Core::finalizeContext() {
	shutdownTaskManager();

	auto threadInitializer = Thread::getThreadInitializer();

	if (threadInitializer)
		threadInitializer->onThreadEnd(this);

	NetworkInterface::finalize();

	Logger::closeGlobalFileLogger();
}

class CoreInitializationTask : public Task {
	Core* core;

public:
	CoreInitializationTask(Core* core) {
		CoreInitializationTask::core = core;
	}

	void run() {
		core->initialize();
	}
};

void Core::start() {
	Reference<Task*> initializerTask = new CoreInitializationTask(this);
	initializerTask->executeInThread();

	run();
}

void Core::shutdownTaskManager() {
	TaskManager* taskMgr = getTaskManager();

    taskManagerShutDown = true;

    if (taskMgr != nullptr)
	    taskMgr->shutdown();

    taskManager = nullptr;
}

/*void Core::scheduleTask(Task* task, uint64 time) {
	TaskManager* taskManager = getTaskManager();
	taskManager->scheduleTask(task, time);
}

void Core::scheduleTask(Task* task, Time& time) {
	TaskManager* taskManager = getTaskManager();
	taskManager->scheduleTask(task, time);
}*/

TaskManager* Core::getTaskManager() {
	if (taskManager.get() == nullptr) {
		if (taskManagerShutDown)
			return nullptr;

#ifdef WITH_STM
		taskManager = new TransactionalTaskManager();
#else
		taskManager = new TaskManagerImpl();
#endif
	}

	return taskManager.get();
}

ObjectBroker* Core::getObjectBroker() {
	return DistributedObjectBroker::instance();
	/*
	if (objectBroker == nullptr)
	#ifdef WITH_STM
		objectBroker = new TransactionalObjectManager();
		//objectBroker = DistributedObjectBroker::instance();
	#else
		objectBroker = DistributedObjectBroker::instance();
	#endif

	return objectBroker;
	 */
}

void Core::outOfMemoryHandler() {
	System::out << "OutOfMemoryException\n";

	//StackTrace::printStackTrace();

	exit(1);
}

Reference<DistributedObject*> Core::lookupObject(uint64 id) {
	return Core::getObjectBroker()->lookUp(id);
}
