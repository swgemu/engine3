/*
 Copyright (C) 2007 <SWGEmu>. All rights reserved.
 Distribution of this file for usage outside of Core3 is prohibited.
 */

#include "AllocationHook.h"

extern "C" void (*__free_hook)(void *__ptr, const void *);
extern "C" void *(*__malloc_hook)(size_t __size, const void *);
extern "C" void *(*__realloc_hook)(void *__ptr, size_t __size, const void *);

void (*__saved_free_hook)(void *__ptr, const void *) = 0;
void *(*__saved_malloc_hook)(size_t __size, const void *) = 0;
void *(*__saved_realloc_hook)(void *__ptr, size_t __size, const void *) = 0;

AllocationHook* instance = NULL;

AllocationHook::AllocationHook() {
	//install();
}

AllocationHook::~AllocationHook() {
	//uninstall();
}

void AllocationHook::install() {
	assert(instance == NULL);

	__saved_malloc_hook = __malloc_hook;
	__saved_free_hook = __free_hook;
	__saved_realloc_hook = __realloc_hook;

	__malloc_hook = mallocHook;
	__free_hook = freeHook;
	__realloc_hook = reallocHook;

	instance = this;
}

void AllocationHook::uninstall() {
	__malloc_hook = __saved_malloc_hook;
	__free_hook = __saved_free_hook;
	__realloc_hook = __saved_realloc_hook;

	instance = NULL;
}

void* AllocationHook::unHookedAllocate(size_t size) {
	uninstall();

	void* ptr = malloc(size);

	//printf("allocated %p\n", ptr);

	install();

	return ptr;
}

void* AllocationHook::unHookedReallocate(void* mem, size_t newsize) {
	uninstall();

	void* ptr = realloc(mem, newsize);

	//printf("reallocated %p -> %p\n", mem, ptr);

	install();

	return ptr;
}

void AllocationHook::unHookedFree(void* mem) {
	uninstall();

	free(mem);

	//printf("freed %p\n", mem);

	install();
}

void* AllocationHook::mallocHook(size_t size, const void* allocator) {
	assert(instance != NULL);

	return instance->onAllocate(size, allocator);
}

void AllocationHook::freeHook(void* ptr, const void* allocator) {
	assert(instance != NULL);

	instance->onFree(ptr, allocator);
}

void* AllocationHook::reallocHook(void* ptr, size_t size, const void* allocator) {
	assert(instance != NULL);

	return instance->onReallocate(ptr, size, allocator);
}
