#ifndef TRANSACTIONABORTEDEXCEPTION_H_
#define TRANSACTIONABORTEDEXCEPTION_H_

#include "system/lang/Exception.h"

#include "Transaction.h"

namespace engine {
  namespace stm {

	class TransactionAbortedException {
	public:
		TransactionAbortedException() {
			Transaction* currentTransaction = Transaction::currentTransaction();

			currentTransaction->abort();
		}
	};

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /*TRANSACTIONABORTEDEXCEPTION_H_*/

