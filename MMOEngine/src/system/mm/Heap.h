/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef HEAP_H_
#define HEAP_H_

#include "MultimapMemoryManager.h"

#include "Allocator.h"

namespace sys {
  namespace mm {

	class Heap : public Allocator {
	protected:
		MultimapMemoryManager* mmapManager;

		void* heapBase;
		size_t heapSize;
		int flags;
		off_t offset;

		Allocator* allocator = nullptr;

	public:
		Heap();

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
