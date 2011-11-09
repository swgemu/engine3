/*
 Copyright (C) 2007 <SWGEmu>. All rights reserved.
 Distribution of this file for usage outside of Core3 is prohibited.
 */

#include <pthread.h>

#include "AllocationHook.h"

#ifndef CLANG_COMPILER
extern "C" void (*__free_hook)(void *__ptr, const void *);
extern "C" void *(*__malloc_hook)(size_t __size, const void *);
extern "C" void *(*__realloc_hook)(void *__ptr, size_t __size, const void *);
#endif

void (*__saved_free_hook)(void *__ptr, const void *) = 0;
void *(*__saved_malloc_hook)(size_t __size, const void *) = 0;
void *(*__saved_realloc_hook)(void *__ptr, size_t __size, const void *) = 0;

AllocationHook* instance = NULL;

pthread_mutex_t mutex;

AllocationHook::AllocationHook() {
	pthread_mutex_init(&mutex, NULL);

	//install();
}

AllocationHook::~AllocationHook() {
	//uninstall();

	pthread_mutex_destroy(&mutex);
}

void AllocationHook::install() {
	assert(instance == NULL);

	instance = this;

#ifndef CLANG_COMPILER
	__saved_malloc_hook = __malloc_hook;
	__saved_free_hook = __free_hook;
	__saved_realloc_hook = __realloc_hook;


	__malloc_hook = mallocHook;
	__free_hook = freeHook;
	__realloc_hook = reallocHook;

#endif

	//printf("hook installed %p\n", __malloc_hook);
}

void AllocationHook::uninstall() {
#ifndef CLANG_COMPILER
	__malloc_hook = __saved_malloc_hook;
	__free_hook = __saved_free_hook;
	__realloc_hook = __saved_realloc_hook;
#endif

	instance = NULL;

	//printf("hook uninstalled %p\n", __saved_malloc_hook);
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

	//printf("freed %p\n", mem);

	free(mem);

	install();
}

void* AllocationHook::mallocHook(size_t size, const void* allocator) {
	void* mem = NULL;

	try {
		pthread_mutex_lock(&mutex);

		assert(instance != NULL);

		mem = instance->onAllocate(size, allocator);
		//mem = instance->unHookedAllocate(size);

		pthread_mutex_unlock(&mutex);
	} catch (...) {
		pthread_mutex_unlock(&mutex);
	}

	return mem;
}

void AllocationHook::freeHook(void* ptr, const void* allocator) {
	try {
		pthread_mutex_lock(&mutex);

		assert(instance != NULL);

		instance->onFree(ptr, allocator);
		//instance->unHookedFree(ptr);

		pthread_mutex_unlock(&mutex);
	} catch (...) {
		pthread_mutex_unlock(&mutex);
	}
}

void* AllocationHook::reallocHook(void* ptr, size_t size, const void* allocator) {
	void* mem = NULL;

	try {
		pthread_mutex_lock(&mutex);

		assert(instance != NULL);

		mem = instance->onReallocate(ptr, size, allocator);
		/*mem = instance->onAllocate(size, allocator);
		memcpy(mem, ptr, size);*/

		//instance->onFree(ptr, allocator);
		//mem = instance->unHookedReallocate(ptr, size);

		pthread_mutex_unlock(&mutex);
	} catch (...) {
		pthread_mutex_unlock(&mutex);
	}

	return mem;
}
