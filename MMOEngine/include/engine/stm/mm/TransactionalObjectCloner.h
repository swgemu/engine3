/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
 */

#ifndef ENGINE_STM_TRANSACTIONALOBJECTCLONER_H_
#define ENGINE_STM_TRANSACTIONALOBJECTCLONER_H_

namespace engine {
  namespace stm {

	template <class O> class TransactionalObjectCloner {
	public:
		static Object* clone(O* object) {
			return dynamic_cast<Object*>(new (TransactionalMemoryManager::instance()->create(sizeof(O))) O(*object));
		}
	};

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALOBJECTCLONER_H_ */
