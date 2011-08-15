/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef HEAP_H_
#define HEAP_H_

#include "DLAllocator.h"

namespace sys {
  namespace mm {

	class Heap {
		void* heapBase;

		size_t heapSize;

		DLAllocator* allocator;

	public:
		Heap(size_t size);

		~Heap();

		void initialize();

		void protect();

		void unprotect();

		void* allocate(size_t size);

		void free(void* mem);
	};

} // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* HEAP_H_ */
