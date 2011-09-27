/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "PTAllocator.h"

#ifdef WITH_STM
#define MSPACES 1
#define USE_DL_PREFIX 1

#include "malloc-2.8.3.h"
#endif

PTAllocator::PTAllocator(void* base, size_t size) {
	dlMspace = NULL;

	dlBase = base;
	dlSize = size;

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

void* PTAllocator::allocate(size_t size) {
#ifdef WITH_STM
	return mspace_malloc((mspace) dlMspace, size);
#else
	return malloc(size);
#endif
}

void* PTAllocator::reallocate(void* mem, size_t newsize) {
#ifdef WITH_STM
	return mspace_realloc((mspace) dlMspace, mem, newsize);
#else
	return realloc(mem, newsize);
#endif
}

void PTAllocator::free(void* mem) {
#ifdef WITH_STM
	mspace_free((mspace) dlMspace, mem);
#else
	free(mem);
#endif
}
