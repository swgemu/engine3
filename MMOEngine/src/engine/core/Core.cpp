/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#include "TaskManagerImpl.h"
#include "engine/stm/task/TransactionalTaskManager.h"

#include "engine/db/mysql/MySqlDatabase.h"

#include "Core.h"

TaskManager* Core::taskManager;

Core::Core() {
	initializeContext();
}

Core::Core(const char* globallogfile) {
	initializeContext();

	Logger::setGlobalFileLogger(globallogfile);
}

Core::~Core() {
	finalizeContext();
}

void Core::initialize() {
	commitTask();
}

void Core::initializeContext() {
	taskManager = NULL;

	std::set_new_handler(outOfMemoryHandler);

	mysql_library_init(0, NULL, NULL);
	mysql_thread_init();

	Thread::initializeThread(this);

	Socket::initialize();

	TaskManager* taskManager = getTaskManager();
	taskManager->initialize();

	taskManager->start();
}

void Core::finalizeContext() {
	TaskManager* taskManager = getTaskManager();
	taskManager->shutdown();

	mysql_thread_end();
	engine::db::mysql::MySqlDatabase::finalizeLibrary();

	NetworkInterface::finalize();

	Logger::closeGlobalFileLogger();
}

void Core::scheduleTask(Task* task, uint64 time) {
	TaskManager* taskManager = getTaskManager();
	taskManager->scheduleTask(task, time);
}

void Core::scheduleTask(Task* task, Time& time) {
	TaskManager* taskManager = getTaskManager();
	taskManager->scheduleTask(task, time);
}

void Core::commitTask() {
#ifdef WITH_STM
	engine::stm::Transaction* transaction = engine::stm::Transaction::currentTransaction();

	if (!transaction->commit())
		throw Exception("unable to commit initialization transaction");
#endif
}

TaskManager* Core::getTaskManager() {
	if (taskManager == NULL)
	#ifdef WITH_STM
		taskManager = new TransactionalTaskManager();
	#else
		taskManager= new TaskManagerImpl();
	#endif

	return taskManager;
}

void Core::outOfMemoryHandler() {
	System::out << "OutOfMemoryException\n";

	//StackTrace::printStackTrace();

	exit(1);
}

