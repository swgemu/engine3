#ifndef TRANSACTIONABORTEDEXCEPTION_H_
#define TRANSACTIONABORTEDEXCEPTION_H_

#include "system/lang/Exception.h"

namespace engine {
  namespace stm {

	class TransactionAbortedException : public Exception {
	public:
		TransactionAbortedException() : Exception("TransactionAbortedException") {
		}
	};

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /*TRANSACTIONABORTEDEXCEPTION_H_*/

