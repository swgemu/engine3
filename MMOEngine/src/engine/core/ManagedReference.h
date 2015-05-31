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

#ifdef CXX11_COMPILER
		ManagedReference(ManagedReference<O>&& ref) : Reference<O>(std::move(ref)) {
		}

		ManagedReference(Reference<O>&& ref) : Reference<O>(std::move(ref)) {
		}
#endif

		ManagedReference(const ManagedWeakReference<O>& r) : Reference<O>() {
			ManagedWeakReference<O>& nonconst = const_cast<ManagedWeakReference<O>&>(r);

			updateObject(nonconst.get());
		}

		ManagedReference(O obj) : Reference<O>(obj) {
		}

		ManagedReference& operator=(const ManagedReference& ref) {
			if (this == &ref)
				return *this;

			updateObject(ref);

			return *this;
		}

#ifdef CXX11_COMPILER
		ManagedReference<O>& operator=(ManagedReference<O>&& ref) {
			if (this == &ref)
				return *this;

			Reference<O>::operator=(std::move(ref));

			return *this;
		}

		ManagedReference<O>& operator=(Reference<O>&& ref) {
			if (this == &ref)
				return *this;

			Reference<O>::operator=(std::move(ref));

			return *this;
		}
#endif

		ManagedReference& operator=(const Reference<O>& ref) {
			updateObject(ref.get());

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

		template<class B>
		ManagedReference<B> castTo() {
			ManagedReference<B> stored;

			stored = dynamic_cast<B>(get());
			return stored;
		}

		inline O get() const {
			return Reference<O>::object;
		}

		inline O getForUpdate() const {
			return Reference<O>::object;
		}

		inline int compareTo(const ManagedReference& ref) const {
			if (((DistributedObject*)Reference<O>::object.get())->_getObjectID() < ((DistributedObject*)ref.Reference<O>::object.get())->_getObjectID())
				return 1;
			else if (((DistributedObject*)Reference<O>::object.get())->_getObjectID() > ((DistributedObject*)ref.Reference<O>::object.get())->_getObjectID())
				return -1;
			else
				return 0;
		}

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


	template<class O> bool ManagedReference<O>::toString(String& str) {
		if (Reference<O>::get() != NULL)
			str = String::valueOf(((DistributedObject*)Reference<O>::get())->_getObjectID());
		else
			str = String::valueOf(0);

		return true;
	}

	template<class O> bool ManagedReference<O>::parseFromString(const String& str, int version) {
		Reference<O> obj = Core::getObjectBroker()->lookUp(UnsignedLong::valueOf(str)).castTo<O>();

		if (obj == NULL) {
			updateObject(NULL);
			return false;
		}

		updateObject(obj.get());

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

		Reference<DistributedObject*> obj = Core::lookupObject(oid);


		if (obj == NULL) {
			updateObject(NULL);
			return false;
		}

		updateObject((O)obj.get());

		return true;
	}

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*MANAGEDREFERENCE_H_*/
