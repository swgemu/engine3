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
	protected:
		uint64 unloadedObjectID;
	public:
		ManagedWeakReference() : WeakReference<O>() {
			unloadedObjectID = 0;
		}

		/*ManagedWeakReference(ManagedWeakReference& ref) : WeakReference<O>(ref) {
		}*/

		ManagedWeakReference(const ManagedWeakReference& ref) : WeakReference<O>(ref) {
			unloadedObjectID = 0;
		}

		ManagedWeakReference(O obj) : WeakReference<O>(obj) {
			unloadedObjectID = 0;
		}

		ManagedWeakReference& operator=(const ManagedWeakReference& ref) {
			if (this == &ref)
				return *this;

			WeakReference<O>::updateObject(ref.object);

			unloadedObjectID = ref.unloadedObjectID;

			return *this;
		}

		O operator=(O obj) {
			WeakReference<O>::updateObject(obj);

			unloadedObjectID = 0;

			return obj;
		}

		inline O get() {
			if (unloadedObjectID != 0) {
				WeakReference<O>::updateObject(dynamic_cast<O>(Core::getObjectBroker()->lookUp(unloadedObjectID)));
				unloadedObjectID = 0;
			}

			return WeakReference<O>::object;
		}

		inline O getForUpdate() {
			if (unloadedObjectID != 0) {
				WeakReference<O>::updateObject(dynamic_cast<O>(Core::getObjectBroker()->lookUp(unloadedObjectID)));
				unloadedObjectID = 0;
			}

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
		O operator->() {
			return get();
		}

		operator O() {
			return get();
		}

	protected:
		void clearObject();

	};

	template<class O> int ManagedWeakReference<O>::compareTo(const ManagedWeakReference& ref) const {
		DistributedObject* thisObj = static_cast<DistributedObject*>(WeakReference<O>::object.get());
		DistributedObject* otherObj = static_cast<DistributedObject*>(ref.WeakReference<O>::object.get());

		uint64 thisOid = (thisObj == NULL) ? unloadedObjectID : thisObj->_getObjectID();
		uint64 otherOid = (otherObj == NULL) ? ref.unloadedObjectID : otherObj->_getObjectID();

		if (thisOid < otherOid)
			return 1;
		else if (thisOid > otherOid)
			return -1;
		else
			return 0;
	}

	template<class O> bool ManagedWeakReference<O>::toString(String& str) {
		if (WeakReference<O>::get() != NULL)
			str = String::valueOf(((DistributedObject*)WeakReference<O>::get())->_getObjectID());
		else if (unloadedObjectID != 0)
			str = String::valueOf(unloadedObjectID);
		else
			str = String::valueOf(0);

		return true;
	}

	template<class O> bool ManagedWeakReference<O>::parseFromString(const String& str, int version) {
		DistributedObject* obj = Core::getObjectBroker()->lookUp(UnsignedLong::valueOf(str));
		unloadedObjectID = 0;

		if (obj == NULL) {
			WeakReference<O>::updateObject(NULL);

			unloadedObjectID = 0;
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
		else if (unloadedObjectID != 0)
			stream->writeLong(unloadedObjectID);
		else
			stream->writeLong(0);

		return true;
	}

	template<class O> bool ManagedWeakReference<O>::parseFromBinaryStream(ObjectInputStream* stream) {
		uint64 oid = stream->readLong();
		unloadedObjectID = 0;

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

	template<class O> void ManagedWeakReference<O>::clearObject() {
		WeakReference<O>::rwlock.wlock();

		DistributedObject* obj = static_cast<DistributedObject*>(WeakReference<O>::object.get());

		if (obj != NULL) {
			unloadedObjectID = obj->_getObjectID();
		} else
			unloadedObjectID = 0;

		WeakReference<O>::object = NULL;

		WeakReference<O>::rwlock.unlock();
	}

  } // namespace core
} // namespace engine

using namespace engine::core;


#endif /* MANAGEDWEAKREFERENCE_H_ */
