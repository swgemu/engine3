/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PTALLOCATOR_H_
#define PTALLOCATOR_H_

#include "system/platform.h"

namespace sys {
  namespace mm {

	class PTAllocator {
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
	};

} // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* PTALLOCATOR_H_ */
