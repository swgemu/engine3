/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
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
	protected:
#ifdef ODB_REFERENCES
		uint64_t loadedOID = 0;
#endif

	public:
		ManagedReference() : Reference<O>() {
		}

		/*ManagedReference(ManagedReference& ref) : Reference<O>(ref) {
		}*/

		ManagedReference(const ManagedReference& ref) : Reference<O>(ref) {
#ifdef ODB_REFERENCES
			loadedOID = ref.loadedOID;
#endif
		}

		ManagedReference(const Reference<O>& r) : Reference<O>(r) {
#ifdef ODB_REFERENCES
			auto obj = r.get();

			loadedOID = obj ? obj->_getObjectID() : 0;
#endif
		}

#ifdef CXX11_COMPILER
		ManagedReference(ManagedReference<O>&& ref) : Reference<O>(std::move(ref)) {
#ifdef ODB_REFERENCES
			loadedOID = ref.loadedOID;
#endif
		}

		ManagedReference(Reference<O>&& ref) : Reference<O>(std::move(ref)) {
#ifdef ODB_REFERENCES
			auto obj = get();

			loadedOID = obj ? obj->_getObjectID() : 0;
#endif
		}
#endif

		ManagedReference(const ManagedWeakReference<O>& r) : Reference<O>() {
			ManagedWeakReference<O>& nonconst = const_cast<ManagedWeakReference<O>&>(r);

			updateObject(nonconst.get());
		}

		ManagedReference(O obj) : Reference<O>(obj) {
#ifdef ODB_REFERENCES
			loadedOID = obj ? obj->_getObjectID() : 0;
#endif
		}

		ManagedReference& operator=(const ManagedReference& ref) {
			if (this == &ref)
				return *this;

#ifdef ODB_REFERENCES
			loadedOID = ref.loadedOID;
#endif
			updateObject(ref);

			return *this;
		}

#ifdef CXX11_COMPILER
		ManagedReference<O>& operator=(ManagedReference<O>&& ref) {
			if (this == &ref)
				return *this;

#ifdef ODB_REFERENCES
			loadedOID = ref.loadedOID;
#endif
			Reference<O>::operator=(std::move(ref));

			return *this;
		}

		ManagedReference<O>& operator=(Reference<O>&& ref) {
			if (this == &ref)
				return *this;

			Reference<O>::operator=(std::move(ref));

#ifdef ODB_REFERENCES
			auto obj = get();
			loadedOID = obj ? obj->_getObjectID() : 0;
#endif

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
		ManagedReference<B> castTo() const {
			ManagedReference<B> stored;

			stored = dynamic_cast<B>(get());
			return stored;
		}

		template<class B>
		ManagedReference<B> castMoveTo() {
			ManagedReference<B> stored;
			auto castedObject = dynamic_cast<B>(get());

			if (castedObject) {
				stored.initializeWithoutAcquire(castedObject);
#ifdef ODB_REFERENCES
				stored.setLoadedOID(this->loadedOID);
				this->loadedOID = 0;
#endif

				this->object = nullptr;
			}

			return stored;
		}

		inline O get() const {
			return Reference<O>::object;
		}

		explicit operator bool() const {
			return get() != nullptr;
		}

		inline O getForUpdate() const {
			return Reference<O>::object;
		}

#ifdef ODB_REFERENCES
		inline void setLoadedOID(uint64 val) {
			loadedOID = val;
		}
#endif

		inline uint64 getObjectID() const {
#ifdef ODB_REFERENCES
			if (!Core::MANAGED_REFERENCE_LOAD)
				return loadedOID;
#endif
			auto val = get();

			if (!val) {
				return 0;
			} else {
				return val->_getObjectID();
			}
		}

		inline int compareTo(const ManagedReference& ref) const {
#ifdef ODB_REFERENCES
			if (!Core::MANAGED_REFERENCE_LOAD) {
				if (loadedOID < ref.loadedOID)
					return 1;
				else if (loadedOID > ref.loadedOID)
					return -1;
				else
					return 0;
			}
#endif
			if (get()->_getObjectID() < ref.get()->_getObjectID())
				return 1;
			else if (get()->_getObjectID() > ref.get()->_getObjectID())
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
#ifdef ODB_REFERENCES
			loadedOID = obj ? obj->_getObjectID() : 0;
#endif
		}

		void updateObject(const ManagedReference& ref) {
			Reference<O>::updateObject(ref.object);

#ifdef ODB_REFERENCES
			loadedOID = ref.object ? ref.object->_getObjectID() : 0;
#endif
		}

//#ifdef WITH_STM
//	private:
//#else
	public:
//#endif
		inline O operator->() const {
			return Reference<O>::object;
		}

		inline operator O() const {
			return Reference<O>::object;
		}

};


	template<class O> bool ManagedReference<O>::toString(String& str) {
		if (Reference<O>::get() != nullptr)
			str = String::valueOf(get()->_getObjectID());
		else
			str = String::valueOf(0);

		return true;
	}

	template<class O> bool ManagedReference<O>::parseFromString(const String& str, int version) {
		auto obj = Core::getObjectBroker()->lookUp(UnsignedLong::valueOf(str)).castTo<O>();

		if (obj == nullptr) {
			updateObject(nullptr);
			return false;
		}

		updateObject(obj.get());

		return true;
	}

	template<class O> bool ManagedReference<O>::toBinaryStream(ObjectOutputStream* stream) {
		stream->writeLong(getObjectID());

		return true;
	}

	template<class O> bool ManagedReference<O>::parseFromBinaryStream(ObjectInputStream* stream) {
#ifdef ODB_REFERENCES
		uint64 oid = loadedOID = stream->readLong();

		if (!Core::MANAGED_REFERENCE_LOAD)
			return true;
#else
		uint64 oid = stream->readLong();
#endif
		auto obj = Core::lookupObject(oid).castTo<O>();

		*this = std::move(obj);

		return get() != nullptr;
	}

	template<class T, class... Args>
	ManagedReference<T*> makeManagedShared(Args&&... args ) {
		return ManagedReference<T*>(new T(std::forward<Args>(args)...));
	}


  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*MANAGEDREFERENCE_H_*/
