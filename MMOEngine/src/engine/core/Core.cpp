/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "ManagedReference.h"
#include "TaskManager.h"

#include "engine/lua/Lua.h"

#include <new>

SynchronizedHashTable<String, ArrayList<String>> Core::properties;
UniqueReference<TaskManager*> Core::taskManager;
bool Core::taskManagerShutDown = false;
bool Core::MANAGED_REFERENCE_LOAD = true;
String Core::engineConfigName;
Core* Core::staticInstance = nullptr;

namespace CoreDetail {
	static Logger logger("Core");
}

//SignalTranslator<SegmentationFault> g_objSegmentationFaultTranslator;

using namespace CoreDetail;

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
	logger.debug("initializing context");

	Core::taskManager = nullptr;
	staticInstance = this;

	std::set_new_handler(outOfMemoryHandler);

	if (threadInitializer)
		threadInitializer->onThreadStart(this);

	Thread::initializeThread(this);

	Socket::initialize();

	TaskManager* taskManager = getTaskManager();
	logger.fatal(taskManager) << "task manager null when initializing context";

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
		logger.error("could not initialize engine properties");
	}

	return count;
}

void Core::finalizeContext() {
	logger.debug("finalizing context");

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
	logger.debug("shutting down task manager");

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
	auto manager = taskManager.get();

	if (manager == nullptr) {
		if (taskManagerShutDown)
			return nullptr;

#ifdef WITH_STM
		taskManager = manager = new TransactionalTaskManager();
#else
		taskManager = manager = new TaskManagerImpl();
#endif
	}

	return manager;
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
	E3_ABORT("OutOfMemoryException");

	//StackTrace::printStackTrace();
}

Reference<DistributedObject*> Core::lookupObject(uint64 id) {
	return Core::getObjectBroker()->lookUp(id);
}

int Core::getIntProperty(const String& key, int defaultValue) {
	const auto val = properties.get(key);

	if (!val.isEmpty()) {
		return Integer::valueOf(val.get(0));
	} else {
		return defaultValue;
	}
}

void Core::setIntProperty(const String& key, int propertyValue) {
	ArrayList<String> values;
	values.emplace(String::valueOf(propertyValue));

	properties.put(key, values);
}

uint64 Core::getLongProperty(const String& key, uint64 defaultValue) {
	const auto val = properties.get(key);

	if (!val.isEmpty()) {
		return UnsignedLong::valueOf(val.get(0));
	} else {
		return defaultValue;
	}
}

String Core::getPropertiesString() {
	StringBuffer buf;
	buf << "\"engine3\":{";

	ReadLocker locker(&Core::properties);

	auto properties = *Core::properties.getHashTable();

	locker.release();

	auto iterator = properties.iterator();

	while (iterator.hasNext()) {
		String* name;
		ArrayList<String>* values;

		iterator.getNextKeyAndValue(name, values);

		buf << "\"" << *name << "\":";

		if (values->size() > 1) {
			buf << "{";

			for (int i = 0; i < values->size(); ++i) {
				const auto& val = values->get(i);

				buf << "\"" << val << "\"";

				if (i + 1 < values->size()) {
					buf << ",";
				}
			}

			buf << "}";
		} else if (values->size()) {
			buf << "\"" << values->get(0) << "\"";
		}

		if (iterator.hasNext()) {
			buf << ",";
		}
	}

	buf << "}";

	return buf.toString();
}

ArrayList<String> Core::getPropertyVector(const String& key) {
	return properties.get(key);
}

String Core::getProperty(const String& key, const String& defaultValue) {
	const auto properties = getPropertyVector(key);

	if (properties.isEmpty()) {
		return defaultValue;
	}

	return properties.get(0);
}

double Core::getDoubleProperty(const String& key, double defaultValue) {
	const auto val = properties.get(key);

	if (!val.isEmpty()) {
		return Double::valueOf(val.get(0));
	} else {
		return defaultValue;
	}
}

