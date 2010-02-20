/*
 * Transaction.h
 *
 *  Created on: 14/02/2010
 *      Author: victor
 */

#ifndef TRANSACTION_H_
#define TRANSACTION_H_

#include <db.h>
#include "Environment.h"
#include "system/platform.h"

namespace engine {
 namespace db {

	 class Transaction {
	 protected:
		 DB_TXN* transaction;

		 Transaction() {
			 transaction = NULL;
		 }

	 public:
		 /**
		  * Cause an abnormal termination of the transaction.
		  *
		  */
		 int abort();

		 /**
		  * End the transaction.
		  */
		 int commit(uint32 flags = 0);

		 /**
		  * End the transaction, not committing synchronously.
		  */
		 inline int commitNoSync() {
			 return commit(DB_TXN_NOSYNC);
		 }

		 /**
		  * End the transaction, committing synchronously.
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
 }
}

using namespace engine::db;


#endif /* TRANSACTION_H_ */
