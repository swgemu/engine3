/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
 */

#ifndef ENGINE_STM_TRANSACTIONALWEAKOBJECTHEADER_H_
#define ENGINE_STM_TRANSACTIONALWEAKOBJECTHEADER_H_

namespace engine {
  namespace stm {
    namespace mm {

	template<class O> class TransactionalWeakObjectHeader : public TransactionalObjectHeader<O> {
			WeakReference<Object*> object;

	public:
			TransactionalWeakObjectHeader(O obj) : TransactionalObjectHeader<O>() {
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

	template<class O> O TransactionalWeakObjectHeader<O>::getObjectForRead(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

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

	template<class O> O TransactionalWeakObjectHeader<O>::getObjectForWrite(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		/*if (transaction != NULL) {
			if (!transaction->isCommited())
				return object;
			else
				throw TransactionAbortedException();

			//return ownerTransaction->getOpenedObject(this);
		} else {*/

		add(handle);
		return getObject();

		//
	}

	template<class O> void TransactionalWeakObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle) {
		object = handle->getObjectLocalCopy()->clone(NULL);

		//ownerTransaction->release();

		//TransactionalObjectHeader<O>::last = NULL;

		TransactionalObjectHeader<O>::ownerTransaction = NULL;
	}

	template<class O> bool TransactionalWeakObjectHeader<O>::isCurrentVersion(Object* obj) {
		if (TransactionalObjectHeader<O>::ownerTransaction != NULL && TransactionalObjectHeader<O>::ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
	}

    } // namespace mm
  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALWEAKOBJECTHEADER_H_ */
