/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef BASEFRAGMENTEDPACKET_H_
#define BASEFRAGMENTEDPACKET_H_

#include "BasePacket.h"

namespace engine {
  namespace service {
    namespace proto {

	class FragmentedPacketParseException : public Exception {
	public:
		FragmentedPacketParseException(const String& msg) : Exception(msg) {

		}
	};

	class BaseFragmentedPacket : public BasePacket {
		BasePacket* singlePacket;

		int totalSize;

	public:
		BaseFragmentedPacket();
		BaseFragmentedPacket(BasePacket* pack);

		~BaseFragmentedPacket();

		bool addFragment(Packet* pack);

		BasePacket* getFragment();

		bool isComplete() const;

		bool hasFragments() const;
	};

    } // namespace proto
  } // namespace service
} // namespace engine

using namespace engine::service::proto;

#endif /*BASEFRAGMENTEDPACKET_H_*/
