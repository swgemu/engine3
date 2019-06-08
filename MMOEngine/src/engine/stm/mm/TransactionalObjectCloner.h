/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ENGINE_STM_TRANSACTIONALOBJECTCLONER_H_
#define ENGINE_STM_TRANSACTIONALOBJECTCLONER_H_

namespace engine {
  namespace stm {

	template <class O> class TransactionalObjectCloner {
	public:
		static Object* clone(O* object) {
		       // KernelCall call;
			void* allocatedAddress = TransactionalMemoryManager::instance()->create(sizeof(O));
			assert(allocatedAddress != nullptr);

			//KernelCall call;
			O* clonedObject = new (allocatedAddress) O(*object);

			Object* downcastedObject = dynamic_cast<Object*>(clonedObject);
			assert(downcastedObject != nullptr);

			return downcastedObject;
		}
	};


  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALOBJECTCLONER_H_ */
