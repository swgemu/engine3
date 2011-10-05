/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "AllocationTracker.h"

#include "DLAllocator.h"

#define USE_DL_PREFIX 1
#define USE_LOCKS 1
#define MSPACES 1
//#define HAVE_MMAP 0
#include "dlmalloc.h"

AllocationTracker tracker(NULL);

void initializeTracker() {
	tracker.install();
}

//void (*__malloc_initialize_hook)(void) = initializeTracker;

DLAllocator::DLAllocator(void* base, size_t size) {
	dlMspace = NULL;

	dlBase = base;
	dlSize = size;

	initialize();
}

DLAllocator::~DLAllocator() {
	destroy();
}

void DLAllocator::initialize() {
	//printf("creating dlallocator at %p with size %llu\n", dlBase, dlSize);

	dlMspace = create_mspace_with_base(dlBase, dlSize, 0);
}

void DLAllocator::destroy() {
	destroy_mspace((mspace) dlMspace);
}

void* DLAllocator::allocate(size_t size) {
	return mspace_malloc((mspace) dlMspace, size);
}

void* DLAllocator::reallocate(void* mem, size_t newsize) {
	return mspace_realloc((mspace) dlMspace, mem, newsize);
}

void DLAllocator::free(void* mem) {
	mspace_free((mspace) dlMspace, mem);
}
