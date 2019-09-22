/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
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
#include "system/thread/atomic/AtomicReference.h"
#include "Environment.h"

namespace engine {
  namespace db {
    namespace berkeley {

	class Transaction {
	protected:
		DB_TXN* transaction{nullptr};

		Transaction() {
		}

	public:
		/**
		 * Cause an abnormal termination of the transaction, deletes this from memory
		 *
		 */
		int abort() NO_THREAD_SAFETY_ANALYSIS;

		/**
		 * End the transaction, deletes this from memory regarding result
		 */
		int commit(uint32 flags = 0) NO_THREAD_SAFETY_ANALYSIS;

		/**
		 * End the transaction, not committing synchronously, deletes this from memory regarding result
		 */
		inline int commitNoSync() NO_THREAD_SAFETY_ANALYSIS {
			return commit(DB_TXN_NOSYNC);
		}

		/**
		 * End the transaction, committing synchronously, deletes this from memory regarding result
		 */
		inline int commitSync() NO_THREAD_SAFETY_ANALYSIS {
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

    } // namespace berkeley
  } // namespace db
} // namespace engine

#endif /* ENGINE_DB_BERKLEY_TRANSACTION_H_ */
