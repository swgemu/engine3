/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ABSTRACTPROTOCOL_H_
#define ABSTRACTPROTOCOL_H_

#include "../../../system/lang.h"

#include "../Message.h"

namespace engine {
  namespace service {
    namespace proto {

	class AbstractProtocol {
	public:
		virtual void prepareSend(Packet* pack) = 0;
	
		virtual bool processRecieve(Packet* pack) = 0;
	};
	
    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*ABSTRACTPROTOCOL_H_*/
