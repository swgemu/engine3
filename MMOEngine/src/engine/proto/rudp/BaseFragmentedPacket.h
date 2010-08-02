/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef BASEFRAGMENTEDPACKET_H_
#define BASEFRAGMENTEDPACKET_H_

#include "RUDPPacket.h"

namespace engine {
  namespace proto {
    namespace rudp {

	class BaseFragmentedPacket : public RUDPPacket {
		RUDPPacket* singlePacket;

		int offset;

	public:
		BaseFragmentedPacket();
		BaseFragmentedPacket(RUDPPacket* pack);

		~BaseFragmentedPacket();

		void addFragment(Packet* pack);

		RUDPPacket* getFragment();

		bool isComplete();

		bool hasFragments();
	};

    } // namespace rudp
  } // namespace proto
} // namespace engine

using namespace engine::proto::rudp;

#endif /*BASEFRAGMENTEDPACKET_H_*/
