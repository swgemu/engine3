/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#include "system/lang/Object.h"

#include "system/thread/atomic/AtomicReference.h"

#include "system/mm/ObjectCloner.h"

#include "system/mm/Heap.h"
#include "system/mm/ProtectedHeap.h"

namespace sys {
  namespace mm {

	class MemoryManager : public Object {
		static AtomicReference<MemoryManager*> inst;

	public:
		static MemoryManager* getInstance() {
			if (inst == nullptr)
				setInstance(new MemoryManager());

			return inst;
		}

		static void setInstance(MemoryManager* manager) {
			inst.set(manager);
		}

		virtual void reclaim(Object* object);
		virtual void create(Object* object);

		static bool protectForRead(void* mem, size_t size);
		static bool protectForWrite(void* mem, size_t size);
		static bool unprotect(void* mem, size_t size);
	};

  } // namespace mm
} // namespace sys

using namespace sys::mm;

#endif /*MEMORYMANAGER_H_*/
