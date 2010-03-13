/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE_STM_TRANSACTION_H_
#define ENGINE_STM_TRANSACTION_H_

#include "system/lang.h"

#include "TransactionalObjectHandle.h"

namespace engine {
  namespace stm {

	class TransactionalObjectHeader;

	class Transaction {
		int status;

		SortedVector<TransactionalObjectHandle*> readOnlyObjects;
		SortedVector<TransactionalObjectHandle*> readWriteObjects;

		static const int UNDECIDED = 0;
		static const int READ_CHECKING = 1;
		static const int ABORTED = 2;
		static const int COMMITTED = 3;

	public:
		Transaction() {
			status = UNDECIDED;
		}

		virtual ~Transaction();

		bool commit();

		void abort();

		bool acquireReadWriteObjects();

		void releaseReadWriteObjects();

		bool validateReadOnlyObjects();

		bool resolveConflict(Transaction* transaction);

		TransactionalObject* openObject(TransactionalObjectHeader* header);

		TransactionalObject* openObjectForWrite(TransactionalObjectHeader* header);

		inline bool isUndecided() {
			return status == UNDECIDED;
		}

		inline bool isReadChecking() {
			return status == READ_CHECKING;
		}

		inline bool isAborted() {
			return status == ABORTED;
		}

		inline bool isCommited() {
			return status == COMMITTED;
		}

		static Transaction* currentTransaction();

	protected:
		bool setState(int currentstate, int newstate);
	};

  } // namespace stm
} // namespace engine

#endif /* ENGINE_STM_TRANSACTION_H_ */
