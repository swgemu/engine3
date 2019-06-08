/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * FraserSTM.h
 *
 *  Created on: 11/11/2011
 *      Author: victor
 */

#ifndef FRASERSTM_H_
#define FRASERSTM_H_

#include "STMAlgorithm.h"

namespace engine {
 namespace stm {

 	 class Transaction;

 	 class FraserSTM : public STMAlgorithm {
 	 public:
 		bool doCommit(Transaction* transaction);
 		bool tryFinishCommit(int desiredStatus, Transaction* transaction);
 		bool validateReadOnlyObjects(Transaction* transaction);

 	 };
 }
}

#endif /* FRASERSTM_H_ */
