/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MANAGEDREFERENCE_H_
#define MANAGEDREFERENCE_H_

#include "system/lang.h"

#include "engine/stm/TransactionalMemoryManager.h"

#include "ManagedObject.h"

#include "engine/orb/DistributedObjectBroker.h"

namespace engine {
  namespace core {

	template<class O> class ManagedReference : public Reference<O> {

	public:
		ManagedReference() : Reference<O>() {
		}

		/*ManagedReference(ManagedReference& ref) : Reference<O>(ref) {
		}*/

		ManagedReference(const ManagedReference& ref) : Reference<O>(ref) {
		}

		ManagedReference(O obj) : Reference<O>(obj) {
		}

		~ManagedReference() {
		}

		ManagedReference& operator=(const ManagedReference& ref) {
			if (this == &ref)
				return *this;

			updateObject(ref);

			return *this;
		}

		O operator=(const O obj) {
			updateObject(obj);

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
			O obj = dynamic_cast<O>(DistributedObjectBroker::instance()->lookUp(UnsignedLong::valueOf(str)));

			if (obj == NULL) {
				updateObject(NULL);
				return false;
			}

			updateObject(obj);

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

			O obj = dynamic_cast<O>(DistributedObjectBroker::instance()->lookUp(oid));


			if (obj == NULL) {
				updateObject(NULL);
				return false;
			}

			updateObject(obj);

			return true;
		}

	protected:
		void updateObject(O obj) {
			Reference<O>::updateObject(obj);
		}

		void updateObject(const ManagedReference& ref) {
			Reference<O>::updateObject(ref.object);
		}
};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*MANAGEDREFERENCE_H_*/
