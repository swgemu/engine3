/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "system/lang/Time.h"

#include "MemoryManager.h"

#include "Heap.h"

#include <sys/mman.h>
#include <limits.h>

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#include "system/thread/Mutex.h"
#include "system/thread/Locker.h"

Mutex mutex;

Heap::Heap() {
	heapBase = NULL;
	heapSize = -1;
}

Heap::~Heap() {
	delete allocator;
}

void Heap::create(size_t size) {
	heapSize = size;

	heapBase = mmap(0, heapSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (heapBase == reinterpret_cast<void*>(-1)) {
		perror("mmap");

		throw Exception();
	}

	//printf("heap created at %p\n", heapBase);

	allocator = new PTAllocator(heapBase, heapSize);
}

Time lastPrintTime;
uint64 sum = 0;

void Heap::protect() {
	Time start;

	MemoryManager::protectForRead(heapBase, heapSize);

	Time end;

	sum += end.getNanoTime() - start.getNanoTime();

	if (lastPrintTime.miliDifference() > 1000) {
		printf("spent %llu time at mprotect\n", sum / 1000 / 1000);
		lastPrintTime.updateToCurrentTime();

		sum = 0;
	}

	//printf("protected %p, %p\n", heapBase, (void*) ((ptrdiff_t) heapBase + heapSize));
	//printf("mprotect time %llu\n", end.getNanoTime() - start.getNanoTime());
}

void Heap::unprotect() {
	Time start;

	MemoryManager::unprotect(heapBase, heapSize);

	Time end;

	sum += end.getNanoTime() - start.getNanoTime();

	//printf("unprotected %p, %p\n", heapBase, (void*) ((ptrdiff_t) heapBase + heapSize));
	//printf("mprotect time %llu\n", end.getNanoTime() - start.getNanoTime());
}

void* Heap::allocate(size_t size) {
	Locker locker(&mutex);
	//return malloc(size);
	return allocator->allocate(size);
}

void* Heap::reallocate(void* mem, size_t size) {
	Locker locker(&mutex);
	//return realloc(mem, size);
	return allocator->reallocate(mem, size);
}

void Heap::free(void* mem) {
	Locker locker(&mutex);
	//free(mem);
	allocator->free(mem);
}

bool Heap::contains(void* mem) {
	ptrdiff_t relativeAddress = (ptrdiff_t) mem - (ptrdiff_t) heapBase;

	return relativeAddress > 0  && relativeAddress <= (ptrdiff_t) heapSize;
}
