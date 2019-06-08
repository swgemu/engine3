/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "engine/engine.h"

#include "TestClass.h"

#include <stdlib.h>

class MemoryPool {
	static const int POOL_SIZE = 50;

	void* addresses[POOL_SIZE];
public:
	MemoryPool() {
		memset(addresses, 0, sizeof(void*) * POOL_SIZE);
	}

	bool add(void* mem) {
		for (int i = 0; i < POOL_SIZE; ++i) {
			if (addresses[i] == 0) {
				//printf("adding %p to %i slot\n", mem, i);
				addresses[i] = mem;
				return true;
			}
		}

		return false;
	}

	void* get() {
		int pos = System::random(POOL_SIZE - 1);
		for (int i = pos; i < POOL_SIZE; ++i) {
			void* mem = addresses[i];

			if (mem != nullptr) {
				addresses[i] = nullptr;

				//printf("got %p from %i slot\n", mem, i);

				return mem;
			}
		}

		for (int i = pos; i >= 0; --i) {
			void* mem = addresses[i];

			if (mem != nullptr) {
				addresses[i] = nullptr;

				//printf("got %p from %i slot\n", mem, i);

				return mem;
			}
		}

		return nullptr;
	}
};

class TestThread : public Thread {
	int threadID;

public:
	TestThread(int id) {
		threadID = id;
	}

	void run() {
		MemoryPool memPool;

		for (int i = 0; i < 10000000; ++i) {
			int addressestoManipulate = System::random(10);

			void* mem = nullptr;
			int size;

			for (int j = 0; j < addressestoManipulate; ++j) {
				switch (System::random(2)) {
				case 0:
					mem = memPool.get();
					if (mem != nullptr) {
						printf("[thread %i] freed %p\n", threadID, mem);

						free(mem);
					}

					break;
				default:
					size = System::random(1000);
					mem = malloc(size);
					memset(mem, 0, size);

					if (!memPool.add(mem))
						free(mem);
					else
						printf("[thread %i] allocated %p %i\n", threadID, mem, size);

					break;
				}
			}

			Thread::yield();
		}
	}
};

void testSTMMemory() {
	Thread* thread1 = new TestThread(1);
	Thread* thread2 = new TestThread(2);
	Thread* thread3 = new TestThread(3);
	Thread* thread4 = new TestThread(4);

	thread1->start();
	thread2->start();
	/*thread3->start();
	thread4->start();*/

	thread1->join();
	thread2->join();
	/*thread3->join();
	thread4->join();*/

	return;

#ifdef WITH_STM
	Reference<Transaction*> transaction = TransactionalMemoryManager::instance()->startTransaction();

	TransactionalReference<TestClass*> object = new TestClass(1);

	TransactionalMemoryManager::commitPureTransaction(transaction);

	object = nullptr;

	//TransactionalMemoryManager::commitPureTransaction();
	//TransactionalMemoryManager::commitPureTransaction();

	Thread::sleep(1000);
#endif

	exit(0);
}
