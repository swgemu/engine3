/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "system/lang/SignalException.h"

#include "engine/engine.h"

#include "system/mm/Heap.h"

#include "TestClass.h"

class SegFault : public Exception {
public:
	SegFault() : Exception() {
		System::out << "Memory protection works!\n";

		throw TransactionAbortedException();
	}

	static int GetSignalNumber() {
		return SIGSEGV;
	}
};

//SignalTranslator<SegFault> segmentationFaultTranslator;

#ifdef WITH_STM
class MemoryTestTask : public Task {
	TransactionalReference<TestClass*> reference;

public:
	MemoryTestTask() {
		reference = new TestClass(1);
	}

	void run() {
		TestClass* object = reference.get();

		printf("writing to read opened object %p\n", object);

		object->increment();
	}
};
#endif


void testMemoryProtection() {
#ifdef WITH_STM
	Reference<Transaction*> transaction = TransactionalMemoryManager::instance()->startTransaction();

	Task* task = new MemoryTestTask();

	Core::getTaskManager()->executeTask(task);

	TransactionalMemoryManager::commitPureTransaction(transaction);

	while (true) {
		Thread::sleep(1000);
	}
#else
	ProtectedHeap kernelHeap;
	kernelHeap.create(512*1024*1024);

	int* mem = (int*) kernelHeap.allocate(8);

	kernelHeap.protect();
	mem[1000] = 12;

	kernelHeap.unprotect();
	*mem = 12;
	printf("Write works\n");
#endif

	/*Thread* thread1 = new TestThread();
	thread1->start();

	Thread* thread2 = new TestThread();
	thread2->start();

	Thread* thread3 = new TestThread();
	thread3->start();

	Thread* thread4 = new TestThread();
	thread4->start();

	thread1->join();*/

	//core.start();

	/*Task* task = new TestTask2();
	task->execute();

	TransactionalMemoryManager::commitPureTransaction();*/

	exit(0);
}
