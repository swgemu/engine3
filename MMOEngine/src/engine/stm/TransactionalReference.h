/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ENGINE_STM_TRANSACTIONALWEAKREFERENCE_H_
#define ENGINE_STM_TRANSACTIONALWEAKREFERENCE_H_

#include "Transaction.h"
#include "TransactionalWeakReference.h"

namespace engine {
namespace stm {

//template <class O> class TransactionalObjectHeader;
//   template<class O> class TransactionalWeakReference;

#ifndef WITH_STM
template<class O> class TransactionalReference : public Reference<O> {
public:
	TransactionalReference() : Reference<O>() {

	}

	TransactionalReference(const TransactionalReference& ref) : Reference<O>(ref) {

	}

	TransactionalReference(const Reference<O>& ref) : Reference<O>(ref) {

	}

	/*TransactionalReference(const TransactionalWeakReference<O>& ref) : Reference<O>(ref) {

	}*/

	TransactionalReference(O obj) : Reference<O>(obj) {

	}

	TransactionalReference& operator=(const TransactionalReference& ref) {
		Reference<O>::operator=(ref);

		return *this;
	}

	TransactionalReference& operator=(const Reference<O> ref) {
		Reference<O>::operator=(ref);

		return *this;
	}

	TransactionalReference& operator=(const TransactionalWeakReference<O>& ref) {
		Reference<O>::operator=(ref);

		return *this;
	}

	O operator=(O obj) {
		return Reference<O>::operator=(obj);
	}
};
#else

	template<class O> class TransactionalReference {
		Reference<TransactionalObjectHeader<O>*> header;

	public:
		TransactionalReference() {
			header = nullptr;
		}

		TransactionalReference(O object) {
			setObject(object);
		}

		TransactionalReference(const TransactionalReference& ref) {
			header = ref.header;
		}

		TransactionalReference(const TransactionalWeakReference<O>& ref) {
			header = ref.header;
		}

		~TransactionalReference() {
			header = nullptr;
		}

		TransactionalReference& operator=(const TransactionalReference& ref) {
			if (this == &ref)
				return *this;

			header = ref.header;

			return *this;
		}

		TransactionalReference& operator=(const TransactionalWeakReference<O>& ref) {
			header = ref.header;

			return *this;
		}

		O operator=(O obj) {
			setObject(obj);

			return obj;
		}

		bool operator==(O obj) {
			return header == getHeader(obj);
		}

		bool operator!=(O obj) {
			return header != getHeader(obj);
		}

		O operator->() {
			O point = getForUpdate();

#ifdef MEMORY_PROTECTION
			ptrdiff_t rel = (ptrdiff_t)point - (ptrdiff_t)0x8000000000;

			assert(!(rel > 0 && rel <= (ptrdiff_t) 0x7e800000));

			//		        assert(!((uint64)point & 0x8000000000));
#endif

			return point;
		}

		operator O() {
			return getForUpdate();
		}

		O get() {
			if (header != nullptr)
				return header->get();
			else
				return nullptr;
		}

		O getForUpdate() {
			if (header != nullptr)
				return header->getForUpdate();
			else
				return nullptr;
		}

		O getForDirty() {
			if (header != nullptr)
				return header->getForDirty();
			else
				return nullptr;
		}

		bool compareAndSet(O oldval, O newval) {
			return header.compareAndSet(getHeader(oldval), getHeader(newval));
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			return getForDirty()->toBinaryStream(stream);
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return getForUpdate()->parseFromBinaryStream(stream);
		}

	protected:
		TransactionalObjectHeader<O>* getHeader(O object) {
			//assert(object != nullptr);
			if (object == nullptr)
				return nullptr;

			return Transaction::currentTransaction()->getHeader(object);
		}

		void setObject(O object) {
			header = getHeader(object);
		}
	};

#endif


} // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALREFERENCE_H_ */
