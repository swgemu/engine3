/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef HEAP_H_
#define HEAP_H_

#include "Allocator.h"

namespace sys {
  namespace mm {

	class Heap {
	protected:
		void* heapBase;
		size_t heapSize;
		int flags;
		off_t offset;

		static int deviceFD;

		Allocator* allocator;

	public:
		Heap();

		~Heap();

		void create(size_t size);

		void* allocate(size_t size);
		void* reallocate(void* mem, size_t size);
		void free(void* mem);

		bool contains(void* mem);

	protected:
		void openDevice(unsigned deviceNumber);

	};

} // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* HEAP_H_ */
