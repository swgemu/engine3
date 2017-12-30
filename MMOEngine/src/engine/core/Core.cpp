/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "ManagedReference.h"

#include "engine/db/mysql/MySqlDatabase.h"


UniqueReference<TaskManager*> Core::taskManager;
bool Core::taskManagerShutDown = false;
//ObjectBroker* Core::objectBroker;

//SignalTranslator<SegmentationFault> g_objSegmentationFaultTranslator;

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

	mysql_library_init(0, nullptr, nullptr);
	mysql_thread_init();

	Thread::initializeThread(this);

	Socket::initialize();

	TaskManager* taskManager = getTaskManager();
	taskManager->setLogLevel(logLevel);
	taskManager->initialize();

	taskManager->start();
}

void Core::finalizeContext() {
    shutdownTaskManager();

	mysql_thread_end();
	engine::db::mysql::MySqlDatabase::finalizeLibrary();

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
