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

  	template<class O> class ManagedReference;

	template<class O> class ManagedWeakReference : public WeakReference<O> {
	protected:
		mutable uint64 savedObjectID;
	public:
		ManagedWeakReference() : WeakReference<O>() {
			savedObjectID = 0;
		}

		/*ManagedWeakReference(ManagedWeakReference& ref) : WeakReference<O>(ref) {
		}*/

		ManagedWeakReference(const ManagedWeakReference& ref) : WeakReference<O>(ref) {
			savedObjectID = ref.savedObjectID;
		}

		ManagedWeakReference(O obj) : WeakReference<O>(obj) {
			savedObjectID = 0;

			if (obj != NULL)
				savedObjectID = obj->_getObjectID();
		}

		ManagedWeakReference& operator=(const ManagedWeakReference& ref) {
			if (this == &ref)
				return *this;

			WeakReference<O>::updateObject(ref.object);

			savedObjectID = ref.savedObjectID;

			return *this;
		}

		O operator=(O obj) {
			WeakReference<O>::updateObject(obj);

			if (obj == NULL)
				savedObjectID = 0;
			else
				savedObjectID = obj->_getObjectID();

			return obj;
		}

		template<class B>
		ManagedReference<B> castTo() {
			ManagedReference<B> stored;
			ManagedReference<O> strong = get();

			stored = dynamic_cast<B>(strong.get());
			return stored;
		}

		inline O getReferenceUnsafe() const {
			return WeakReference<O>::getReferenceUnsafe();
		}

		inline bool operator==(O obj) {
			return getReferenceUnsafe() == obj;
		}

		inline bool operator!=(O obj) {
			return getReferenceUnsafe() != obj;
		}

		inline bool operator!=(const ManagedWeakReference<O>& r) {
			return getReferenceUnsafe() != r.getReferenceUnsafe();
		}

		inline bool operator==(const ManagedWeakReference<O>& r) {
			return getReferenceUnsafe() == r.getReferenceUnsafe();
		}

		inline ManagedReference<O> get() {
			ManagedReference<O> strongRef = WeakReference<O>::get();

			if (savedObjectID != 0 && strongRef == NULL) {
				strongRef = dynamic_cast<O>(Core::getObjectBroker()->lookUp(savedObjectID));

				WeakReference<O>::updateObject(strongRef.get());
			}

			return strongRef;
		}

		inline ManagedReference<O> getForUpdate() {
			ManagedReference<O> strongRef = WeakReference<O>::get();

			if (savedObjectID != 0 && strongRef == NULL) {
				strongRef = dynamic_cast<O>(Core::getObjectBroker()->lookUp(savedObjectID));

				WeakReference<O>::updateObject(strongRef.get());
			}

			return strongRef;
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

	};

	template<class O> int ManagedWeakReference<O>::compareTo(const ManagedWeakReference& ref) const {
		uint64 thisOid = savedObjectID;
		uint64 otherOid = ref.savedObjectID;

		if (thisOid < otherOid)
			return 1;
		else if (thisOid > otherOid)
			return -1;
		else
			return 0;
	}

	template<class O> bool ManagedWeakReference<O>::toString(String& str) {
		str = String::valueOf(savedObjectID);

		return true;
	}

	template<class O> bool ManagedWeakReference<O>::parseFromString(const String& str, int version) {
		uint64 oid = UnsignedLong::valueOf(str);

		DistributedObject* obj = Core::getObjectBroker()->lookUp(oid);
		savedObjectID = 0;

		if (obj == NULL) {
			WeakReference<O>::updateObject(NULL);

			return false;
		}

		O castedObject = dynamic_cast<O>(obj);

		if (castedObject == NULL) {
			return false;
		}

		savedObjectID = oid;

		WeakReference<O>::updateObject(castedObject);

		return true;
	}

	template<class O> bool ManagedWeakReference<O>::toBinaryStream(ObjectOutputStream* stream) {
		stream->writeLong(savedObjectID);

		return true;
	}

	template<class O> bool ManagedWeakReference<O>::parseFromBinaryStream(ObjectInputStream* stream) {
		uint64 oid = stream->readLong();
		savedObjectID = 0;

		DistributedObject* obj = Core::getObjectBroker()->lookUp(oid);

		if (obj == NULL) {
			WeakReference<O>::updateObject(NULL);
			return false;
		}

		O castedObject = dynamic_cast<O>(obj);

		if (castedObject == NULL)
			return false;

		savedObjectID = oid;

		WeakReference<O>::updateObject(castedObject);

		return true;
	}

  } // namespace core
} // namespace engine

using namespace engine::core;


#endif /* MANAGEDWEAKREFERENCE_H_ */
