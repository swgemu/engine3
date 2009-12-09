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

	template<class O> class ManagedReference : public Reference<O> {
	public:
		ManagedReference() : Reference<O>() {
		}

		ManagedReference(const ManagedReference& ref) : Reference<O>(ref) {
		}

		ManagedReference(O obj) : Reference<O>(obj) {
		}

		void operator=(const ManagedReference& ref) {
			Reference<O>::setObject(ref.object);
		}

		O operator=(O obj) {
			Reference<O>::updateObject(obj);

			return obj;
		}

		int compareTo(const ManagedReference& ref) const {
			if (Reference<O>::object->_getObjectID() < ref.Reference<O>::object->_getObjectID())
				return 1;
			else if (Reference<O>::object->_getObjectID() > ref.Reference<O>::object->_getObjectID())
				return -1;
			else
				return 0;
		}

		bool toString(String& str) {
			if (Reference<O>::get() != NULL)
				str = String::valueOf((Reference<O>::get())->_getObjectID());
			else
				str = String::valueOf(0);

			return true;
		}

		bool parseFromString(const String& str, int version = 0) {
			DistributedObject* obj = DistributedObjectBroker::instance()->lookUp(UnsignedLong::valueOf(str));

			Reference<O>::updateObject((O) obj);

			if (obj == NULL)
				return false;

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			O object = Reference<O>::get();

			if (object != NULL)
				stream->writeLong(object->_getObjectID());
			else
				stream->writeLong(0);

			return true;
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			uint64 oid = stream->readLong();

			O obj = (O) DistributedObjectBroker::instance()->lookUp(oid);
			*this = obj;

			if (obj == NULL)
				return false;

			return true;
		}

	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*MANAGEDREFERENCE_H_*/
