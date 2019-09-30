/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * TransactionConfig.h
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#pragma once

#include <db.h>

#include "system/platform.h"

namespace engine {
  namespace db {
    namespace berkeley {

	class TransactionConfig {
		uint32 transactionFlags;

	public:
		static TransactionConfig DEFAULT;

	public:
		TransactionConfig() {
			transactionFlags = 0;
		}

		/**
		 * Configure the transaction to not write or synchronously flush the log it when commits.
		 */
		void setNoSync(bool noSync) {
			if (noSync)
				transactionFlags |= DB_TXN_NOSYNC;
			else
				transactionFlags &= ~DB_TXN_NOSYNC;
		}

		/**
		 * Configure the transaction to not wait if a lock request cannot be immediately granted.
		 */
		void setNoWait(bool noWait) {
			if (noWait)
				transactionFlags |= DB_TXN_NOWAIT;
			else
				transactionFlags &= ~DB_TXN_NOWAIT;
		}

		/**
		 * Configure the transaction for read committed isolation.
		 */
		void setReadCommitted(bool readCommitted) {
			if (readCommitted)
				transactionFlags |= DB_READ_COMMITTED;
			else
				transactionFlags &= ~DB_READ_COMMITTED;
		}

		/**
		 * Configure read operations performed by the transaction to return modified but not yet committed data.
		 */
		void setReadUncommitted(bool readUncommitted) {
			if (readUncommitted)
				transactionFlags |= DB_READ_UNCOMMITTED;
			else
				transactionFlags &= ~DB_READ_UNCOMMITTED;
		}

		/**
		 * This transaction will execute with snapshot isolation.
		 */
		void setSnapshot(bool snapshot) {
			if (snapshot)
				transactionFlags |= DB_TXN_SNAPSHOT;
			else
				transactionFlags &= ~DB_TXN_SNAPSHOT;
		}

		/**
		 * Configure the transaction to write and synchronously flush the log it when commits.
		 */
		void setSync(bool sync) {
			if (sync)
				transactionFlags |= DB_TXN_SYNC;
			else
				transactionFlags &= ~DB_TXN_SYNC;
		}

		/**
        * Configure the transaction to wait if a lock request cannot be immediately granted.
        */
		void setWait(bool wait) {
			if (wait)
				transactionFlags |= DB_TXN_WAIT;
			else
				transactionFlags &= ~DB_TXN_WAIT;
		}

		/**
		 * Configure the transaction to write but not synchronously flush the log it when commits.
		 */
		void setWriteNoSync(bool writeNoSync) {
			if (writeNoSync)
				transactionFlags |= DB_TXN_WRITE_NOSYNC;
			else
				transactionFlags &= ~DB_TXN_WRITE_NOSYNC;
		}

		inline uint32 getFlags() const {
			return transactionFlags;
		}
	};

    } // namespace berkeley
  } // namespace db
} // namespace engine

