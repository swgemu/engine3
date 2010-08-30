/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MANAGEDWEAKREFERENCE_H_
#define MANAGEDWEAKREFERENCE_H_

#include "system/lang.h"

#include "ManagedObject.h"

#include "../orb/DistributedObjectBroker.h"

namespace engine {
  namespace core {

	template<class O> class ManagedWeakReference : public WeakReference<O> {
	public:
		ManagedWeakReference() : WeakReference<O>() {
		}

		/*ManagedWeakReference(ManagedWeakReference& ref) : WeakReference<O>(ref) {
		}*/

		ManagedWeakReference(const ManagedWeakReference& ref) : WeakReference<O>(ref) {
		}

		ManagedWeakReference(O obj) : WeakReference<O>(obj) {
		}

		ManagedWeakReference& operator=(const ManagedWeakReference& ref) {
			if (this == &ref)
				return *this;

			WeakReference<O>::updateObject(ref.object);

			return *this;
		}

		O operator=(O obj) {
			WeakReference<O>::updateObject(obj);

			return obj;
		}

		inline O get() const {
			return WeakReference<O>::object;
		}

		inline O getForUpdate() const {
			return WeakReference<O>::object;
		}

		int compareTo(const ManagedWeakReference& ref) const {
			if (WeakReference<O>::object->_getObjectID() < ref.WeakReference<O>::object->_getObjectID())
				return 1;
			else if (WeakReference<O>::object->_getObjectID() > ref.WeakReference<O>::object->_getObjectID())
				return -1;
			else
				return 0;
		}

		bool toString(String& str) {
			if (WeakReference<O>::get() != NULL)
				str = String::valueOf((WeakReference<O>::get())->_getObjectID());
			else
				str = String::valueOf(0);

			return true;
		}

		bool parseFromString(const String& str, int version = 0) {
			DistributedObject* obj = DistributedObjectBroker::instance()->lookUp(UnsignedLong::valueOf(str));

			WeakReference<O>::updateObject((O) obj);

			if (obj == NULL)
				return false;

			return true;
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			O object = WeakReference<O>::get();

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

	private:
		O operator->() const {
			return WeakReference<O>::object;
		}

		operator O() const {
			return WeakReference<O>::object;
		}

	};

  } // namespace core
} // namespace engine

using namespace engine::core;


#endif /* MANAGEDWEAKREFERENCE_H_ */
