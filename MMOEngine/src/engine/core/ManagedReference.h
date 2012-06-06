/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MANAGEDREFERENCE_H_
#define MANAGEDREFERENCE_H_

#include "system/lang.h"

#include "Core.h"

#include "engine/stm/TransactionalMemoryManager.h"

#include "ManagedObject.h"

namespace engine {
  namespace core {

    template<class O> class ManagedWeakReference;

	template<class O> class ManagedReference : public Reference<O> {

	public:
		ManagedReference() : Reference<O>() {
		}

		/*ManagedReference(ManagedReference& ref) : Reference<O>(ref) {
		}*/

		ManagedReference(const ManagedReference& ref) : Reference<O>(ref) {
		}

		ManagedReference(const Reference<O>& r) : Reference<O>(r) {
		}

		ManagedReference(ManagedWeakReference<O> r) : Reference<O>(r.get()) {

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

		ManagedReference& operator=(ManagedWeakReference<O> ref) {
			updateObject(ref.get());

			return *this;
		}

		O operator=(const O obj) {
			updateObject(obj);

			return obj;
		}

		inline O get() const {
			return Reference<O>::object;
		}

		inline O getForUpdate() const {
			return Reference<O>::object;
		}

		int compareTo(const ManagedReference& ref) const;

		bool toString(String& str);

		bool parseFromString(const String& str, int version = 0);

		bool toBinaryStream(ObjectOutputStream* stream);

		bool parseFromBinaryStream(ObjectInputStream* stream);

	protected:
		void updateObject(O obj) {
			Reference<O>::updateObject(obj);
		}

		void updateObject(const ManagedReference& ref) {
			Reference<O>::updateObject(ref.object);
		}

//#ifdef WITH_STM
//	private:
//#else
	public:
//#endif
		O operator->() const {
			return Reference<O>::object;
		}

		operator O() const {
			return Reference<O>::object;
		}

};

	template<class O> int ManagedReference<O>::compareTo(const ManagedReference& ref) const {
		if (((DistributedObject*)Reference<O>::object.get())->_getObjectID() < ((DistributedObject*)ref.Reference<O>::object.get())->_getObjectID())
			return 1;
		else if (((DistributedObject*)Reference<O>::object.get())->_getObjectID() > ((DistributedObject*)ref.Reference<O>::object.get())->_getObjectID())
			return -1;
		else
			return 0;
	}

	template<class O> bool ManagedReference<O>::toString(String& str) {
		if (Reference<O>::get() != NULL)
			str = String::valueOf(((DistributedObject*)Reference<O>::get())->_getObjectID());
		else
			str = String::valueOf(0);

		return true;
	}

	template<class O> bool ManagedReference<O>::parseFromString(const String& str, int version) {
		O obj = dynamic_cast<O>(Core::getObjectBroker()->lookUp(UnsignedLong::valueOf(str)));

		if (obj == NULL) {
			updateObject(NULL);
			return false;
		}

		updateObject(obj);

		return true;
	}

	template<class O> bool ManagedReference<O>::toBinaryStream(ObjectOutputStream* stream) {
		O object = Reference<O>::get();

		if (object != NULL)
			stream->writeLong(((DistributedObject*)object)->_getObjectID());
		else
			stream->writeLong(0);

		return true;
	}

	template<class O> bool ManagedReference<O>::parseFromBinaryStream(ObjectInputStream* stream) {
		uint64 oid = stream->readLong();

		O obj = (O)(Core::getObjectBroker()->lookUp(oid));


		if (obj == NULL) {
			updateObject(NULL);
			return false;
		}

		updateObject((O)obj);

		return true;
	}

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*MANAGEDREFERENCE_H_*/
