/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "system/lang/Time.h"

#include "MemoryManager.h"

#include "Heap.h"

#include <limits.h>

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

Heap::Heap(size_t size) {
	heapSize = size;

	initialize();
}

Heap::~Heap() {
	delete allocator;
}

void Heap::initialize() {
	heapBase = malloc(heapSize + PAGESIZE - 1);
	if (heapBase != NULL) {
		perror("Couldn't allocate heap");
		return;
	}

	heapBase = (void *)(((size_t) heapBase + PAGESIZE-1) & ~(PAGESIZE-1));

	MemoryManager::unprotect(heapBase, heapSize);

	allocator = new DLAllocator(heapBase, heapSize);
}

void Heap::protect() {
	Time start;

	MemoryManager::protectForRead(heapBase, heapSize);

	Time end;

	printf("mprotect time %llu\n", end.getNanoTime() - start.getNanoTime());
}

void Heap::unprotect() {
	Time start;

	MemoryManager::unprotect(heapBase, heapSize);

	Time end;

	printf("mprotect time %llu\n", end.getNanoTime() - start.getNanoTime());
}

void* Heap::allocate(size_t size) {
	return allocator->allocate(size);
}

void Heap::free(void* mem) {
	allocator->free(mem);
}
