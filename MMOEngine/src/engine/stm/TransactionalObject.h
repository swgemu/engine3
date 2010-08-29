/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALOBJECT_H_
#define ENGINE_STM_TRANSACTIONALOBJECT_H_

#include "system/lang.h"

#include "TransactionalMemoryManager.h"

namespace engine {
  namespace stm {

	class TransactionalObject {
		virtual TransactionalObject* clone() {
			return NULL;
		}

	public:
		virtual ~TransactionalObject() {

		}
	};

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* ENGINE_STM_TRANSACTIONALOBJECT_H_ */
