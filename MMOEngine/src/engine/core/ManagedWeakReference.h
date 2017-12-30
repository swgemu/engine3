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

	template<class O> class ManagedWeakReference : public WeakReference<O>, public Variable {
	protected:
		mutable uint64 savedObjectID;
	public:
		ManagedWeakReference() : WeakReference<O>(), Variable() {
			savedObjectID = 0;
		}

		ManagedWeakReference(const ManagedWeakReference& ref) : WeakReference<O>(ref), Variable() {
			savedObjectID = ref.savedObjectID;
		}

		ManagedWeakReference(StrongAndWeakReferenceCount* p, uint64 oid) : WeakReference<O>(p), Variable() {
			savedObjectID = oid;
		}

#ifdef CXX11_COMPILER
		ManagedWeakReference(ManagedWeakReference<O>&& ref) : WeakReference<O>(std::move(ref)), Variable(),
				savedObjectID(ref.savedObjectID) {
			ref.savedObjectID = 0;
		}
#endif

		ManagedWeakReference(O obj) : WeakReference<O>(obj), Variable() {
			savedObjectID = 0;

			if (obj != nullptr)
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

			if (obj == nullptr)
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

		template<class B>
		ManagedWeakReference<B> staticCastToWeak() {
			StrongAndWeakReferenceCount* p = WeakReference<O>::safeRead();

			ManagedWeakReference<B> ref(p, savedObjectID);

			WeakReference<O>::release(p);

			return ref;
		}

		inline O getReferenceUnsafe() {
			O ref = WeakReference<O>::getReferenceUnsafeStaticCast();

			if (ref == nullptr && savedObjectID != 0) {
				Reference<DistributedObject*> tempObj = Core::lookupObject(savedObjectID);
				ref = dynamic_cast<O>(tempObj.get());

				WeakReference<O>::updateObject(ref);
			}

			return ref;
		}

		inline bool operator==(O obj) {
			O ref = getReferenceUnsafe();

			if (ref == nullptr && savedObjectID != 0) {
				if (obj == nullptr)
					return savedObjectID == 0;
				else
					return savedObjectID == obj->_getObjectID();
			} else
				return ref == obj;
		}

		inline bool operator!=(O obj) {
			O ref = getReferenceUnsafe();

			if (ref == nullptr && savedObjectID != 0) {
				if (obj == nullptr)
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
			ManagedReference<O> strongRef = WeakReference<O>::get();

			if (strongRef == nullptr && savedObjectID != 0) {
				Reference<DistributedObject*> tempObj = Core::lookupObject(savedObjectID);
				strongRef = dynamic_cast<O>(tempObj.get());

				WeakReference<O>::updateObject(strongRef.get());

				if (strongRef == NULL) {
					savedObjectID = 0;
				}
			}

			return strongRef;
		}

		inline ManagedReference<O> getForUpdate() {
			ManagedReference<O> strongRef = WeakReference<O>::get();

			if (savedObjectID != 0 && strongRef == nullptr) {
				Reference<DistributedObject*> tempObj = Core::lookupObject(savedObjectID);
				strongRef = dynamic_cast<O>(tempObj.get());

				WeakReference<O>::updateObject(strongRef.get());

				if (strongRef == NULL) {
					savedObjectID = 0;
				}
			}

			return strongRef;
		}

		inline uint64 getSavedObjectID() const {
			return savedObjectID;
		}

		int compareTo(const ManagedWeakReference& ref) const;

		bool toString(String& str);

		bool parseFromString(const String& str, int version = 0);

		bool toBinaryStream(ObjectOutputStream* stream);

		bool parseFromBinaryStream(ObjectInputStream* stream);

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

		if (obj == nullptr) {
			WeakReference<O>::updateObject(nullptr);

			return false;
		}

		O castedObject = dynamic_cast<O>(obj);

		if (castedObject == nullptr) {
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

		WeakReference<O>::updateObject((O)nullptr);

		return true;
	}

  } // namespace core
} // namespace engine

using namespace engine::core;


#endif /* MANAGEDWEAKREFERENCE_H_ */
