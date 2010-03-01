/*
 * Transaction.h
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#ifndef ENGINE_DB_BERKLEY_TRANSACTION_H_
#define ENGINE_DB_BERKLEY_TRANSACTION_H_

#include <db.h>

#include "system/platform.h"

#include "Environment.h"

namespace engine {
  namespace db {
    namespace berkley {

	class Transaction {
	protected:
		DB_TXN* transaction;

		Transaction() {
			transaction = NULL;
		}

	public:
		/**
		 * Cause an abnormal termination of the transaction, deletes this from memory
		 *
		 */
		int abort();

		/**
		 * End the transaction, deletes this from memory regarding result
		 */
		int commit(uint32 flags = 0);

		/**
		 * End the transaction, not committing synchronously, deletes this from memory regarding result
		 */
		inline int commitNoSync() {
			return commit(DB_TXN_NOSYNC);
		}

		/**
		 * End the transaction, committing synchronously, deletes this from memory regarding result
		 */
		inline int commitSync() {
			return commit(DB_TXN_SYNC);
		}

		inline DB_TXN* getDBTXN() const {
			return transaction;
		}

		inline DB_TXN** getDBTXNPTR() {
			return &transaction;
		}

		friend class Environment;

	};

    } // namespace berkley
  } // namespace db
} // namespace engine

#endif /* ENGINE_DB_BERKLEY_TRANSACTION_H_ */
