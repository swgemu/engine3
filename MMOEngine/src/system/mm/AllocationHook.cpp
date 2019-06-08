/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#include <pthread.h>

#include "AllocationHook.h"

#ifdef PLATFORM_MAC

void *(*system_malloc)(malloc_zone_t *zone, size_t size);
void *(*system_realloc)(malloc_zone_t *zone, void *ptr, size_t size);
void (*system_free)(malloc_zone_t *zone, void *ptr);
void (*system_free_definite_size)(malloc_zone_t *zone, void *ptr, size_t size);

extern "C" malloc_zone_t **malloc_zones;
extern "C" unsigned malloc_num_zones_allocated;

#else
extern "C" void (*__free_hook)(void *__ptr, const void *);
extern "C" void *(*__malloc_hook)(size_t __size, const void *);
extern "C" void *(*__realloc_hook)(void *__ptr, size_t __size, const void *);

void (*__saved_free_hook)(void *__ptr, const void *) = 0;
void *(*__saved_malloc_hook)(size_t __size, const void *) = 0;
void *(*__saved_realloc_hook)(void *__ptr, size_t __size, const void *) = 0;

#endif

AllocationHook* instance = nullptr;

pthread_mutex_t mutex;

AllocationHook::AllocationHook() {
	pthread_mutex_init(&mutex, nullptr);

	//install();
}

AllocationHook::~AllocationHook() {
	//uninstall();

	pthread_mutex_destroy(&mutex);
}

void AllocationHook::install() {
	assert(instance == nullptr);

	instance = this;

#ifndef PLATFORM_MAC
	__saved_malloc_hook = __malloc_hook;
	__saved_free_hook = __free_hook;
	__saved_realloc_hook = __realloc_hook;


	__malloc_hook = mallocHook;
	__free_hook = freeHook;
	__realloc_hook = reallocHook;

#else
	malloc_zone_t *dz = malloc_default_zone();

	if (dz->version >= 8) {
		unsigned protect_size = malloc_num_zones_allocated * sizeof(malloc_zone_t *);

		vm_protect(mach_task_self(), (uintptr_t)malloc_zones, protect_size, 0, VM_PROT_READ | VM_PROT_WRITE);//remove the write protection
	}

	system_malloc = dz->malloc;
	system_realloc = dz->realloc;
	system_free = dz->free;
	system_free_definite_size = dz->free_definite_size;

	dz->malloc = &mallocHook;
	dz->free = &freeHook;
	dz->realloc = &reallocHook;
	dz->free_definite_size = &freeDefiniteSizeHook;

	if (dz->version <= 8) {
		unsigned protect_size = malloc_num_zones_allocated * sizeof(malloc_zone_t *);

		vm_protect(mach_task_self(), (uintptr_t)malloc_zones, protect_size, 0, VM_PROT_READ);//put the write protection back
	}

#endif

	//printf("hook installed %p\n", __malloc_hook);
}

void AllocationHook::uninstall() {
#ifndef PLATFORM_MAC
	__malloc_hook = __saved_malloc_hook;
	__free_hook = __saved_free_hook;
	__realloc_hook = __saved_realloc_hook;
#else
	malloc_zone_t *dz = malloc_default_zone();

	if (dz->version >= 8) {
		unsigned protect_size = malloc_num_zones_allocated * sizeof(malloc_zone_t *);

		vm_protect(mach_task_self(), (uintptr_t)malloc_zones, protect_size, 0, VM_PROT_READ | VM_PROT_WRITE);//remove the write protection
	}

	dz->malloc = system_malloc;
	dz->free = system_free;
	dz->realloc = system_realloc;
	dz->free_definite_size = system_free_definite_size;

	if (dz->version <= 8) {
		unsigned protect_size = malloc_num_zones_allocated * sizeof(malloc_zone_t *);

		vm_protect(mach_task_self(), (uintptr_t)malloc_zones, protect_size, 0, VM_PROT_READ);//put the write protection back
	}
#endif

	instance = nullptr;

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

#ifndef PLATFORM_MAC
void* AllocationHook::mallocHook(size_t size, const void* allocator) {
	void* mem = nullptr;

	try {
		pthread_mutex_lock(&mutex);

		assert(instance != nullptr);

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

		assert(instance != nullptr);

		instance->onFree(ptr, allocator);
		//instance->unHookedFree(ptr);

		pthread_mutex_unlock(&mutex);
	} catch (...) {
		pthread_mutex_unlock(&mutex);
	}
}

void* AllocationHook::reallocHook(void* ptr, size_t size, const void* allocator) {
	void* mem = nullptr;

	try {
		pthread_mutex_lock(&mutex);

		assert(instance != nullptr);

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

#else

void* AllocationHook::mallocHook(_malloc_zone_t* zone, size_t size) {
	void* mem = nullptr;

	try {
		pthread_mutex_lock(&mutex);

		assert(instance != nullptr);

		mem = instance->onAllocate(size, zone);
		//mem = instance->unHookedAllocate(size);

		pthread_mutex_unlock(&mutex);
	} catch (...) {
		pthread_mutex_unlock(&mutex);
	}

	return mem;
}

void* AllocationHook::reallocHook(malloc_zone_t *zone, void *ptr, size_t size) {
	void* mem = nullptr;

	try {
		pthread_mutex_lock(&mutex);

		assert(instance != nullptr);

		mem = instance->onReallocate(ptr, size, zone);
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

void AllocationHook::freeHook(malloc_zone_t *zone, void *ptr) {
	try {
		pthread_mutex_lock(&mutex);

		assert(instance != nullptr);

		instance->onFree(ptr, zone);
		//instance->unHookedFree(ptr);

		pthread_mutex_unlock(&mutex);
	} catch (...) {
		pthread_mutex_unlock(&mutex);
	}
}

void AllocationHook::freeDefiniteSizeHook(malloc_zone_t *zone, void *ptr, size_t size) {
	freeHook(zone, ptr);
}

#endif
