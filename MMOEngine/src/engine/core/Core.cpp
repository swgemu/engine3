/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "ManagedReference.h"

#include "engine/lua/Lua.h"

SynchronizedHashTable<String, ArrayList<String>> Core::properties;
UniqueReference<TaskManager*> Core::taskManager;
bool Core::taskManagerShutDown = false;
bool Core::MANAGED_REFERENCE_LOAD = true;
String Core::engineConfigName;
Core* Core::staticInstance = nullptr;

//SignalTranslator<SegmentationFault> g_objSegmentationFaultTranslator;
//
//
Core::Core(int logLevel) {
	initializeContext(logLevel);
}

Core::Core(const char* globallogfile, const char* name, int logLevel) {
	Logger::setGlobalFileLogger(globallogfile);
	Core::engineConfigName = name;

	initializeContext(logLevel);
}

Core::~Core() {
	finalizeContext();
}

void Core::initializeContext(int logLevel) {
	taskManager = nullptr;
	staticInstance = this;

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

Core* Core::getCoreInstance() {
	return staticInstance;
}

void Core::parsePropertyData(const String& className, const char* name, LuaObject& table) {
	ArrayList<String> values;
	auto luaState = table.getLuaState();

	if (!lua_isstring(luaState, -1)) {
		auto size = table.getTableSize();

		for (int i = 1; i <= size; ++i) {
			values.emplace(table.getStringAt(i));
		}

		table.pop();
	} else {
		values.emplace(Lua::getStringParameter(table.getLuaState()));
	}

	Core::properties.put(std::move((className + ".") + name), std::move(values));
}

int Core::initializeProperties(const String& className) {
	static Mutex guard;

	Locker locker(&guard);
	int count = 0;

	try {
		static Lua lua = []() {
			Lua lua;

			lua.init();

			lua.runFile("conf/engine3.lua");
			lua.runFile("engine3.lua");
			lua.runFile("conf/" + engineConfigName + ".lua");

			return lua;
		} ();

		auto obj = lua.getGlobalObject(className);

		if (!obj.isValidTable()) {
			return 0;
		}

		auto L = obj.getLuaState();
		lua_pushnil(L);

		while (lua_next(L, -2) != 0) {
			int type = lua_type(L, -2);

			if (type == LUA_TSTRING) {
				size_t len = 0;
				const char* varName = lua_tolstring(L, -2, &len);

				parsePropertyData(className, varName, obj);
				++count;
			} else {
				lua_pop(L, 1);
			}
		}

		obj.pop();

	} catch (...) {
		Logger::console.error("could not initialize engine properties");
	}

	return count;
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

int Core::getIntProperty(const String& key, int defaultValue) {
	const auto& val = properties.get(key);

	if (!val.isEmpty()) {
		return Integer::valueOf(val.get(0));
	} else {
		return defaultValue;
	}
}

uint64 Core::getLongProperty(const String& key, uint64 defaultValue) {
	const auto& val = properties.get(key);

	if (!val.isEmpty()) {
		return UnsignedLong::valueOf(val.get(0));
	} else {
		return defaultValue;
	}
}

ArrayList<String> Core::getPropertyVector(const String& key) {
	return properties.get(key);
}

String Core::getProperty(const String& key, const String& defaultValue) {
	auto properties = getPropertyVector(key);

	if (properties.isEmpty()) {
		return defaultValue;
	}

	return properties.get(0);
}

double Core::getDoubleProperty(const String& key, double defaultValue) {
	auto val = properties.get(key);

	if (!val.isEmpty()) {
		return Double::valueOf(val.get(0));
	} else {
		return defaultValue;
	}
}

