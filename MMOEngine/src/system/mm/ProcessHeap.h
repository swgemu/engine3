/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef PROTECTEDHEAP_H_
#define PROTECTEDHEAP_H_

#include "Heap.h"

namespace sys {
  namespace mm {

	class ProcessHeap : public Heap {
		int heapID;

	public:
		ProcessHeap();
		~ProcessHeap();

		void select();

	};

} // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /* PROTECTEDHEAP_H_ */
