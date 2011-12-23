/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef HEAP_H_
#define HEAP_H_

#include "system/platform.h"

#include "system/thread/atomic/AtomicInteger.h"

#include "Allocator.h"

namespace sys {
  namespace mm {

	class Heap : public Allocator {
	protected:
		void* heapBase;
		size_t heapSize;
		int flags;
		off_t offset;

		int deviceFD;

		Allocator* allocator;

		static AtomicInteger heapCount;

	public:
		Heap();
		Heap(int fd);

		~Heap();

		void create(size_t size);

		void* allocate(size_t size);
		void* reallocate(void* mem, size_t size);
		void free(void* mem);

		size_t sizeOf(void* mem);

		bool contains(void* mem);

		void setShared();
		void setPrivate();

		void setAnonymous();

		Allocator* getAllocator() {
			return allocator;
		}
};

} // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* HEAP_H_ */
