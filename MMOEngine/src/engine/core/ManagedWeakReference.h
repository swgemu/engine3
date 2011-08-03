/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MANAGEDWEAKREFERENCE_H_
#define MANAGEDWEAKREFERENCE_H_

#include "system/lang.h"

#include "Core.h"

#include "ManagedObject.h"

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

		int compareTo(const ManagedWeakReference& ref) const;

		bool toString(String& str);

		bool parseFromString(const String& str, int version = 0);

		bool toBinaryStream(ObjectOutputStream* stream);

		bool parseFromBinaryStream(ObjectInputStream* stream);

//#ifdef WITH_STM
//	private:
//#else
	public:
//#endif
		O operator->() const {
			return WeakReference<O>::object;
		}

		operator O() const {
			return WeakReference<O>::object;
		}

	};

	template<class O> int ManagedWeakReference<O>::compareTo(const ManagedWeakReference& ref) const {
		if (((DistributedObject*)WeakReference<O>::object.get())->_getObjectID() < ((DistributedObject*)ref.WeakReference<O>::object.get())->_getObjectID())
			return 1;
		else if (((DistributedObject*)WeakReference<O>::object.get())->_getObjectID() > ((DistributedObject*)ref.WeakReference<O>::object.get())->_getObjectID())
			return -1;
		else
			return 0;
	}

	template<class O> bool ManagedWeakReference<O>::toString(String& str) {
		if (WeakReference<O>::get() != NULL)
			str = String::valueOf(((DistributedObject*)WeakReference<O>::get())->_getObjectID());
		else
			str = String::valueOf(0);

		return true;
	}

	template<class O> bool ManagedWeakReference<O>::parseFromString(const String& str, int version) {
		DistributedObject* obj = Core::getObjectBroker()->lookUp(UnsignedLong::valueOf(str));

		if (obj == NULL) {
			WeakReference<O>::updateObject(NULL);
			return false;
		}

		O castedObject = dynamic_cast<O>(obj);

		WeakReference<O>::updateObject(castedObject);

		if (castedObject == NULL)
			return false;

		return true;
	}

	template<class O> bool ManagedWeakReference<O>::toBinaryStream(ObjectOutputStream* stream) {
		O object = WeakReference<O>::get();

		if (object != NULL)
			stream->writeLong(((DistributedObject*)object)->_getObjectID());
		else
			stream->writeLong(0);

		return true;
	}

	template<class O> bool ManagedWeakReference<O>::parseFromBinaryStream(ObjectInputStream* stream) {
		uint64 oid = stream->readLong();

		DistributedObject* obj = Core::getObjectBroker()->lookUp(oid);

		if (obj == NULL) {
			WeakReference<O>::updateObject(NULL);
			return false;
		}

		O castedObject = (O)(obj);

		WeakReference<O>::updateObject((O)castedObject);

		if (castedObject == NULL)
			return false;

		return true;
	}

  } // namespace core
} // namespace engine

using namespace engine::core;


#endif /* MANAGEDWEAKREFERENCE_H_ */
