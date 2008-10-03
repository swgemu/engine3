/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASEMULTIPACKET_H_
#define BASEMULTIPACKET_H_

#include "BasePacket.h"

namespace engine {
  namespace service {
    namespace proto {

	class BaseMultiPacket : public BasePacket {
		BasePacket* singlePacket;
		
	public:
		BaseMultiPacket(BasePacket* pack);
	
		~BaseMultiPacket();
		
		bool add(BasePacket* pack);

		BasePacket* getPacket();
		
	protected:
		void insertPacket(BasePacket* pack);
		
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASEMULTIPACKET_H_*/
