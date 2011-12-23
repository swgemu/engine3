/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MEMORYMAPPEDFILE_H_
#define MEMORYMAPPEDFILE_H_

#include "Heap.h"

namespace sys {
  namespace mm {

	class MemoryMappedFile : public Heap {
		const char* fileName;

	public:
		MemoryMappedFile(const char* name);

	protected:
		void openDevice();

	};

} // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* MEMORYMAPPEDFILE_H_ */
