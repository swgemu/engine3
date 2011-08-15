/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef DLALLOCATOR_H_
#define DLALLOCATOR_H_

#include "system/platform.h"

namespace sys {
  namespace mm {

	class DLAllocator {
		void* dlBase;
		size_t dlSize;

		void* dlMspace;

	public:
		DLAllocator(void* base, size_t size);

		~DLAllocator();

		void initialize();

		void destroy();

		void* allocate(size_t size);

		void free(void* mem);
	};

} // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* DLALLOCATOR_H_ */
