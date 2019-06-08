/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
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

			currentTransaction->setState(Transaction::ABORTED);
		}
	};

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /*TRANSACTIONABORTEDEXCEPTION_H_*/

