/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * TransactionalBaseClientManager.h
 *
 *  Created on: 08/06/2011
 *      Author: victor
 */

#ifndef TRANSACTIONALBASECLIENTMANAGER_H_
#define TRANSACTIONALBASECLIENTMANAGER_H_

#include "engine/util/Command.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseClient;
	class BasePacket;

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

namespace engine {
  namespace stm {

  	class TransactionalBaseClientManager : public Command, public Logger {
  		//ThreadLocal<VectorMap<BaseClient*, Vector<BasePacket*>*>* > bufferedPackets;
  		ThreadLocal<Vector<BasePacket*>* > bufferedPackets;

  	public:
  		TransactionalBaseClientManager();

  		void initialize();

  		void sendPacket(BasePacket* packet, BaseClient* baseClient);

  		void execute();

  		void undo();

  	protected:
  		Vector<BasePacket*>* getLocalBufferedPackets();
  	  };

  } // namespace stm
} // namespace engine

using namespace engine::stm;

#endif /* TRANSACTIONALBASECLIENTMANAGER_H_ */
