/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef PTALLOCATOR_H_
#define PTALLOCATOR_H_

#include "Allocator.h"

namespace sys {
  namespace mm {

	class PTAllocator : public Allocator {
		void* dlBase;
		size_t dlSize;

		void* dlMspace;

	public:
		PTAllocator(void* base, size_t size);

		~PTAllocator();

		void initialize();

		void destroy();

		void* allocate(size_t size);

		void* reallocate(void* mem, size_t newsize);

		void free(void* mem);

		void printStatistics();
	};

  } // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* PTALLOCATOR_H_ */
