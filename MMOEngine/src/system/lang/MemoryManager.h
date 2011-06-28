/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#include "Object.h"

#include "system/thread/atomic/AtomicReference.h"

namespace sys {
  namespace lang {

	class MemoryManager : public Object {
		static AtomicReference<MemoryManager*> inst;

	public:
		static MemoryManager* getInstance() {
			if (inst == NULL)
				setInstance(new MemoryManager());

			return inst;
		}

		static void setInstance(MemoryManager* manager) {
			inst.set(manager);
		}

		virtual void reclaim(Object* object);
		virtual void create(Object* object);
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*MEMORYMANAGER_H_*/
