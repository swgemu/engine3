/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include "AllocationTracker.h"

#include "PTAllocator.h"

#ifdef WITH_STM
#define USE_STARTER 0
#define MSPACES 1
#define USE_DL_PREFIX 1

#include "malloc-2.8.3.h"
#endif

PTAllocator::PTAllocator(void* base, size_t size) {
	dlMspace = nullptr;

	dlBase = base;
	dlSize = size;

	//new AllocationTracker();

	initialize();
}

PTAllocator::~PTAllocator() {
	destroy();
}

void PTAllocator::initialize() {
	//printf("creating dlallocator at %p with size %llu\n", dlBase, dlSize);
#ifdef WITH_STM
	dlMspace = create_mspace_with_base(dlBase, dlSize, 0);
#endif
}

void PTAllocator::destroy() {
#ifdef WITH_STM
	destroy_mspace((mspace) dlMspace);
#endif
}

Time nextPrint;

void* PTAllocator::allocate(size_t size) {
#ifdef WITH_STM
	void* mem = mspace_malloc((mspace) dlMspace, size);
	//printf("allocated %p\n", mem);

	if (nextPrint.isPast()) {
		printStatistics();
		nextPrint.addMiliTime(10000);
	}

	return mem;
#else
	return malloc(size);
#endif
}

void* PTAllocator::reallocate(void* mem, size_t newsize) {
#ifdef WITH_STM
	void* mem2 = mspace_realloc((mspace) dlMspace, mem, newsize);

	//printf("reallocated %p to %p\n", mem, mem2);

	return mem2;
#else
	return realloc(mem, newsize);
#endif
}

void PTAllocator::free(void* mem) {
#ifdef WITH_STM
	mspace_free((mspace) dlMspace, mem);

	//printf("freeing %p\n", mem);
#else
	::free(mem);
#endif
}

void PTAllocator::printStatistics() {
	//mspace_malloc_stats((mspace) dlMspace);
}
