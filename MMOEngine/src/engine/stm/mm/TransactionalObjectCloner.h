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
		       // KernelCall call;
			void* allocatedAddress = TransactionalMemoryManager::instance()->create(sizeof(O));
			assert(allocatedAddress != NULL);

			//KernelCall call;
			O* clonedObject = new (allocatedAddress) O(*object);

			Object* downcastedObject = dynamic_cast<Object*>(clonedObject);
			assert(downcastedObject != NULL);

			return downcastedObject;
		}
	};


  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALOBJECTCLONER_H_ */
