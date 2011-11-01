/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
 */

#ifndef ENGINE_STM_TRANSACTIONALSTRONGOBJECTHEADER_H_
#define ENGINE_STM_TRANSACTIONALSTRONGOBJECTHEADER_H_

namespace engine {
  namespace stm {
    namespace mm {

	template<class O> class TransactionalStrongObjectHeader : public TransactionalObjectHeader<O> {
		Reference<Object*> object;

	public:
		TransactionalStrongObjectHeader(O obj) : TransactionalObjectHeader<O>() {
			setObject(obj);
		}

		bool isCurrentVersion(Object* obj);

		O getForDirty() {
			return getObject();
		}

	protected:
		O getObject() {
			return dynamic_cast<O>(object.get());
		}

		void setObject(O obj) {
			assert(obj != NULL);
			assert(object == NULL);

			object = obj;
			TransactionalObjectHeader<O>::createObject();

			assert(object != NULL);
		}

		bool hasObject(Object* obj) const {
			return object == obj;
		}

		void releaseObject(TransactionalObjectHandle<O>* handle);

		O getObjectForRead(TransactionalObjectHandle<O>* handle);
		O getObjectForWrite(TransactionalObjectHandle<O>* handle);

	};

	template<class O> O TransactionalStrongObjectHeader<O>::getObjectForRead(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		assert(object != NULL);

		if (transaction != NULL) {
			if (!transaction->isCommited())
				return getObject();
			else
				throw TransactionAbortedException();

			//return ownerTransaction->getOpenedObject(this);
		} else {
			//add(handle);
			return getObject();
		}
	}

	template<class O> O TransactionalStrongObjectHeader<O>::getObjectForWrite(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		/*if (transaction != NULL) {
		 	 if (!transaction->isCommited())
		 	 	 return object;
		 	 else
		 	 	 throw TransactionAbortedException();

		 	 //return ownerTransaction->getOpenedObject(this);
		 } else {*/

		add(handle);

		assert(object != NULL);
		return getObject();
		//}
	}

	template<class O> void TransactionalStrongObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle) {
		object = handle->getObjectLocalCopy()->clone(NULL);

		assert(object != NULL);

		//TransactionalObjectHeader<O>::last = NULL;

		//ownerTransaction->release();

		TransactionalObjectHeader<O>::ownerTransaction = NULL;
	}

	template<class O> bool TransactionalStrongObjectHeader<O>::isCurrentVersion(Object* obj) {
		if (TransactionalObjectHeader<O>::ownerTransaction != NULL && TransactionalObjectHeader<O>::ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
	}

    } // namespace mm
  } // namespace stm
} // namespace engine

using namespace engine::stm::mm;

#endif /* ENGINE_STM_TRANSACTIONALSTRONGOBJECTHEADER_H_ */
