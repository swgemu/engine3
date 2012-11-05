/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef PROTECTEDHEAP_H_
#define PROTECTEDHEAP_H_

#include "Heap.h"

namespace sys {
  namespace mm {

	class ProtectedHeap : public Heap {
		volatile unsigned protectionLevel;

	public:
		ProtectedHeap();

		void protect();
		void unprotect();

	};

} // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* PROTECTEDHEAP_H_ */
