/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
 */

#ifndef ENGINE_STM_TRANSACTIONALSTRONGOBJECTHEADER_H_
#define ENGINE_STM_TRANSACTIONALSTRONGOBJECTHEADER_H_

namespace engine {
  namespace stm {

	template<class O> class TransactionalStrongObjectHeader : public TransactionalObjectHeader<O> {
		Reference<Object*> object;

	public:
		TransactionalStrongObjectHeader() : TransactionalObjectHeader<O>() {
			object = NULL;
		}

		TransactionalStrongObjectHeader(O obj) : TransactionalObjectHeader<O>() {
			setObject(obj);
		}

		bool isCurrentVersion(Object* obj);

		O getForDirty() {
			return dynamic_cast<O>(object.get());
		}

	protected:
		void setObject(O obj) {
			if (object == NULL)
				object = dynamic_cast<Object*>(obj)->clone();
			else
				object = dynamic_cast<Object*>(obj)->clone(NULL);

			assert(object != NULL);
		}

		bool isNull() {
			return object == NULL;
		}

		bool hasObject(Object* obj) const {
			return object == obj;
		}

		void releaseObject(TransactionalObjectHandle<O>* handle);

		Object* getObjectForRead(TransactionalObjectHandle<O>* handle);
		Object* getObjectForWrite(TransactionalObjectHandle<O>* handle);

	};

	template<class O> Object* TransactionalStrongObjectHeader<O>::getObjectForRead(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		if (transaction != NULL) {
			if (!transaction->isCommited())
				return object;
			else
			throw TransactionAbortedException();
			//return ownerTransaction->getOpenedObject(this);
		} else {
			//add(handle);
			return object;
		}
	}

	template<class O> Object* TransactionalStrongObjectHeader<O>::getObjectForWrite(TransactionalObjectHandle<O>* handle) {
		Transaction* transaction = TransactionalObjectHeader<O>::ownerTransaction;

		/*if (transaction != NULL) {
		 	 if (!transaction->isCommited())
		 	 	 return object;
		 	 else
		 	 	 throw TransactionAbortedException();

		 	 //return ownerTransaction->getOpenedObject(this);
		 } else {*/

		add(handle);
		return object;

		//}
	}

	template<class O> void TransactionalStrongObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle) {
		object = handle->getObjectLocalCopy()->clone(NULL);

		//TransactionalObjectHeader<O>::last = NULL;

		//ownerTransaction->release();

		TransactionalObjectHeader<O>::ownerTransaction = NULL;
	}

	template<class O> bool TransactionalStrongObjectHeader<O>::isCurrentVersion(Object* obj) {
		if (TransactionalObjectHeader<O>::ownerTransaction != NULL && TransactionalObjectHeader<O>::ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
	}

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTIONALSTRONGOBJECTHEADER_H_ */
