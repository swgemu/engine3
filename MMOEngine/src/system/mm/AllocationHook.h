/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ALLOCATIONHOOK_H_
#define ALLOCATIONHOOK_H_

#include "system/platform.h"

#ifdef PLATFORM_MAC
#include <malloc/malloc.h>
#include <mach/mach_init.h>
#include <mach/mach_vm.h>
#include <sys/mman.h>
#include <mach/mach.h>
#endif

namespace sys {
  namespace mm {

	class AllocationHook {
	public:
		AllocationHook();

		virtual ~AllocationHook();

		void install();
		void uninstall();

		virtual void* onAllocate(size_t size, const void* allocator) = 0;
		virtual void onFree(void* ptr, const void* allocator) = 0;
		virtual void* onReallocate(void* ptr, size_t size, const void* allocator) = 0;

		void* unHookedAllocate(size_t size);
		void* unHookedReallocate(void* mem, size_t newsize);
		void unHookedFree(void* mem);

	private:
#ifndef PLATFORM_MAC
		static void* mallocHook(size_t size, const void* allocator);
		static void freeHook(void* ptr, const void* allocator);
		static void* reallocHook(void* ptr, size_t size, const void* allocator);
#else
		static void* mallocHook(_malloc_zone_t* zone , size_t size);
		static void* reallocHook(malloc_zone_t *zone, void *ptr, size_t size);
		static void freeHook(malloc_zone_t *zone, void *ptr);
		static void freeDefiniteSizeHook(malloc_zone_t *zone, void *ptr, size_t size);
#endif
	};

  } // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* ALLOCATIONHOOK_H_ */
