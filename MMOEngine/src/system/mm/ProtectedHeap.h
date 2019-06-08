/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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
