/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_
#define ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_

#include "TransactionalObject.h"

namespace engine {
  namespace stm {

	class TransactionalObjectHeader;

	class TransactionalObjectHandle {
		TransactionalObjectHeader* header;

		TransactionalObject* object;
		TransactionalObject* objectCopy;

	public:
		TransactionalObjectHandle(TransactionalObjectHeader* header);

		virtual ~TransactionalObjectHandle();

		TransactionalObjectHeader* getObjectHeader() {
			return header;
		}

		bool hasObjectChanged();

		int compareTo(TransactionalObjectHandle* handle) {
			if (this == handle)
				return 0;
			else if (this < handle)
				return 1;
			else
				return -1;
		}

		TransactionalObject* getObjectLocalCopy() {
			return objectCopy;
		}
	};

  } // namespace stm
} // namespace engine


#endif /* ENGINE_STM_TRANSACTIONALOBJECTHANDLE_H_ */
