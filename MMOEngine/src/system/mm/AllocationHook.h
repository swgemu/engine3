/*
 Copyright (C) 2007 <SWGEmu>. All rights reserved.
 Distribution of this file for usage outside of Core3 is prohibited.
 */

#ifndef ALLOCATIONHOOK_H_
#define ALLOCATIONHOOK_H_

#include "system/platform.h"

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
		static void* mallocHook(size_t size, const void* allocator);
		static void freeHook(void* ptr, const void* allocator);
		static void* reallocHook(void* ptr, size_t size, const void* allocator);
	};

  } // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* ALLOCATIONHOOK_H_ */
