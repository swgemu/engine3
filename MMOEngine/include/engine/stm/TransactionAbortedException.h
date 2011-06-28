#ifndef TRANSACTIONABORTEDEXCEPTION_H_
#define TRANSACTIONABORTEDEXCEPTION_H_

#include "system/lang/Exception.h"

#include "Transaction.h"

namespace engine {
  namespace stm {

	class TransactionAbortedException {
		bool retryTransaction;
	public:
		TransactionAbortedException(bool restart = true) {
			retryTransaction = restart;

			Transaction* currentTransaction = Transaction::currentTransaction();

			currentTransaction->abort();
		}

		inline bool getRetryTransaction() {
			return retryTransaction;
		}
	};

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /*TRANSACTIONABORTEDEXCEPTION_H_*/

