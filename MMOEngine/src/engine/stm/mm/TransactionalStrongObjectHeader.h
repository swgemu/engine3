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
		Reference<O> object;

	public:
		TransactionalStrongObjectHeader() : TransactionalObjectHeader<O>() {
			object = NULL;
		}

		TransactionalStrongObjectHeader(O obj) : TransactionalObjectHeader<O>() {
			setObject(obj);
		}

		bool isCurrentVersion(O obj);

		O getForDirty() {
			return dynamic_cast<O>(object.get());
		}

	protected:
		void setObject(O obj) {
			assert(object == NULL );

			object = obj;
			TransactionalObjectHeader<O>::createObject();

			//object = dynamic_cast<O>(obj->clone(NULL));

			assert(object != NULL);
		}

		bool isNull() {
			return object == NULL;
		}

		bool hasObject(O obj) const {
			bool wtf = (object == obj);
			return wtf;
		}

		void releaseObject(TransactionalObjectHandle<O>* handle);

		O getObjectForRead(TransactionalObjectHandle<O>* handle);
		O getObjectForWrite(TransactionalObjectHandle<O>* handle);

	};

	template<class O> O TransactionalStrongObjectHeader<O>::getObjectForRead(TransactionalObjectHandle<O>* handle) {
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
		return object;

		//}
	}

	template<class O> void TransactionalStrongObjectHeader<O>::releaseObject(TransactionalObjectHandle<O>* handle) {
		object = dynamic_cast<O>(handle->getObjectLocalCopy()->clone(NULL));

		//TransactionalObjectHeader<O>::last = NULL;

		//ownerTransaction->release();

		TransactionalObjectHeader<O>::ownerTransaction = NULL;
	}

	template<class O> bool TransactionalStrongObjectHeader<O>::isCurrentVersion(O obj) {
		if (TransactionalObjectHeader<O>::ownerTransaction != NULL && TransactionalObjectHeader<O>::ownerTransaction != Transaction::currentTransaction())
			return false;

		return object == obj;
	}

    } // namespace mm
  } // namespace stm
} // namespace engine

using namespace engine::stm::mm;

#endif /* ENGINE_STM_TRANSACTIONALSTRONGOBJECTHEADER_H_ */
