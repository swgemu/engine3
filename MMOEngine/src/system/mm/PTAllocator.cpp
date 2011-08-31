/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "PTAllocator.h"

#define MSPACES 1
#define USE_DL_PREFIX 1

#include "malloc-2.8.3.h"

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

	dlMspace = create_mspace_with_base(dlBase, dlSize, 0);
}

void PTAllocator::destroy() {
	destroy_mspace((mspace) dlMspace);
}

void* PTAllocator::allocate(size_t size) {
	return mspace_malloc((mspace) dlMspace, size);
}

void* PTAllocator::reallocate(void* mem, size_t newsize) {
	return mspace_realloc((mspace) dlMspace, mem, newsize);
}

void PTAllocator::free(void* mem) {
	mspace_free((mspace) dlMspace, mem);
}
