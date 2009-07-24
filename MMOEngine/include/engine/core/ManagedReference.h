/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MANAGEDREFERENCE_H_
#define MANAGEDREFERENCE_H_

#include "../../system/lang.h"

#include "ManagedObject.h"

#include "../orb/DistributedObjectBroker.h"

namespace engine {
  namespace core {

	template<class O> class ManagedReference : public ReferenceSlot<O> {
	public:
		ManagedReference() : ReferenceSlot<O>() {
		}

		ManagedReference(const ManagedReference& ref) : ReferenceSlot<O>(ref) {
		}

		ManagedReference(O obj) : ReferenceSlot<O>(obj) {
		}

		void operator=(const ManagedReference& ref) {
			ReferenceSlot<O>::setObject(ref.object);
		}

		void operator=(O obj) {
			ReferenceSlot<O>::updateObject(obj);
		}

		int compareTo(const ManagedReference& ref) const {
			if (ReferenceSlot<O>::object->_getObjectID() < ref.ReferenceSlot<O>::object->_getObjectID())
				return 1;
			else if (ReferenceSlot<O>::object->_getObjectID() > ref.ReferenceSlot<O>::object->_getObjectID())
				return -1;
			else
				return 0;
		}

		bool toString(String& str) {
			if (ReferenceSlot<O>::object != NULL)
				str = ReferenceSlot<O>::object->_getName();

			return true;
		}

		bool parseFromString(const String& str, int version = 0) {
			DistributedObject* obj = DistributedObjectBroker::instance()->lookUp(str);

			ReferenceSlot<O>::updateObject((O) obj);

			if (obj == NULL)
				return false;

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			String name;

			if (ReferenceSlot<O>::object != NULL)
				name = ReferenceSlot<O>::object->_getName();

			name.toBinaryStream(stream);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			String name;
			name.parseFromBinaryStream(stream);

			DistributedObject* obj = DistributedObjectBroker::instance()->lookUp(name);

			ReferenceSlot<O>::updateObject((O) obj);

			if (obj == NULL)
				return false;

			return true;
		}

	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*MANAGEDREFERENCE_H_*/
