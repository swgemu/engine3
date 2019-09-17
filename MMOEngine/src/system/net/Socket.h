/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SOCKET_H_
#define SOCKET_H_

#include "system/platform.h"

#include "system/io/FileDescriptor.h"

#include "SocketAddress.h"

namespace sys {
  namespace io {
	  class Stream;
  }

  namespace net {
	class Packet;

	class Socket : public FileDescriptor {
	public:
		Socket();
		Socket(int handle);

		static void initialize();

		void bindTo(const SocketAddress* bindpoint);

		void listen(int maxconn);

		Socket* accept();

		Socket* accept(SocketAddress* addr);

		void connect(const SocketAddress* bindpoint);

		bool read(Packet* pack);

		bool readAppend(Stream* pack);

		int readFrom(Packet* pack, SocketAddress* addr);

		int send(Packet* pack);

		int sendTo(Packet* pack, const SocketAddress* addr);

		void close();

		int shutdown(int how);

		void setLingering(int time);

		void disableLingering();

#ifdef PLATFORM_WIN
	private:
		static bool winsockInitialized;
#endif

	};

  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*SOCKET_H_*/
