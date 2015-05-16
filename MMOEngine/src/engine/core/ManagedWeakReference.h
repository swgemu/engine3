/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MANAGEDWEAKREFERENCE_H_
#define MANAGEDWEAKREFERENCE_H_

#include "system/lang.h"

#include "engine/orb/object/DistributedObject.h"
#include "ManagedReference.h"
#include "Core.h"

#include "ManagedObject.h"
#include "engine/orb/db/DOBObjectManager.h"

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

#ifdef CXX11_COMPILER
		ManagedWeakReference(ManagedWeakReference<O>&& ref) : WeakReference<O>(std::move(ref)),
				savedObjectID(ref.savedObjectID) {
			ref.savedObjectID = 0;
		}
#endif

		ManagedWeakReference(O obj) : WeakReference<O>(obj) {
			savedObjectID = 0;

			if (obj != NULL)
				savedObjectID = obj->_getObjectID();
		}

		ManagedWeakReference& operator=(const ManagedWeakReference& ref) {
			if (this == &ref)
				return *this;

			WeakReference<O>::operator=(ref);

			savedObjectID = ref.savedObjectID;

			return *this;
		}

#ifdef CXX11_COMPILER
		ManagedWeakReference<O>& operator=(ManagedWeakReference<O>&& ref) {
			if (this == &ref)
				return *this;

			WeakReference<O>::operator=(std::move(ref));

			savedObjectID = ref.savedObjectID;

			ref.savedObjectID = 0;

			return *this;
		}
#endif

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
			return WeakReference<O>::getReferenceUnsafeStaticCast();
		}

		inline bool operator==(O obj) {
			O ref = getReferenceUnsafe();

			if (ref == NULL && savedObjectID != 0) {
				if (obj == NULL)
					return savedObjectID == 0;
				else
					return savedObjectID == obj->_getObjectID();
			} else
				return ref == obj;
		}

		inline bool operator!=(O obj) {
			O ref = getReferenceUnsafe();

			if (ref == NULL && savedObjectID != 0) {
				if (obj == NULL)
					return savedObjectID != 0;
				else
					return savedObjectID != obj->_getObjectID();
			} else
				return ref != obj;
		}

		inline bool operator!=(const ManagedWeakReference<O>& r) {
			return savedObjectID != r.savedObjectID;
		}

		inline bool operator==(const ManagedWeakReference<O>& r) {
			return savedObjectID == r.savedObjectID;
		}

		inline ManagedReference<O> get() {
		        //Locker locker(Core::getObjectBroker()->getObjectManager());
		        
			ManagedReference<O> strongRef = WeakReference<O>::get();
			
			//locker.release();

			if (savedObjectID != 0 && strongRef == NULL) {
				Reference<DistributedObject*> tempObj = Core::lookupObject(savedObjectID);
				strongRef = dynamic_cast<O>(tempObj.get());

				WeakReference<O>::updateObject(strongRef.get());
			}

			return strongRef;
		}

		inline ManagedReference<O> getForUpdate() {
		        //Locker locker(Core::getObjectBroker()->getObjectManager());
		        
			ManagedReference<O> strongRef = WeakReference<O>::get();
			
			//locker.release();

			if (savedObjectID != 0 && strongRef == NULL) {
				Reference<DistributedObject*> tempObj = Core::lookupObject(savedObjectID);
				strongRef = dynamic_cast<O>(tempObj.get());

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

		Reference<DistributedObject*> obj = Core::lookupObject(oid);
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
		savedObjectID = oid;
/*
		DistributedObject* obj = Core::getObjectBroker()->lookUp(oid);

		if (obj == NULL) {
			WeakReference<O>::updateObject(NULL);
			return false;
		}

		O castedObject = dynamic_cast<O>(obj);

		if (castedObject == NULL)
			return false;

		savedObjectID = oid;
		*/

		WeakReference<O>::updateObject(NULL);

		return true;
	}

  } // namespace core
} // namespace engine

using namespace engine::core;


#endif /* MANAGEDWEAKREFERENCE_H_ */
