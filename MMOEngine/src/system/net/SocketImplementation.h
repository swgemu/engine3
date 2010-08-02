/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SOCKETIMPLEMENTATION_H_
#define SOCKETIMPLEMENTATION_H_

#include "../platform.h"

#include "SocketAddress.h"

namespace sys {
  namespace net {
	
	class SocketImplementation;

	class SocketImplementationInitializer {
		bool initialized;

		SocketImplementationInitializer();

		void initialize();

		friend class SocketImplementation;
	};

	class Packet;

	class SocketImplementation {
	protected:
		int sock;

		uint64 timeout;
		struct timeval tv;

		static SocketImplementationInitializer socketInitializer;

	public:
		static const int STREAM_SOCKET = 1;

		static const int DATAGRAM_SOCKET = 2;

		static const int DEFAULT_TIMEOUT = 10;

	public:
		SocketImplementation(int type = 0);

		SocketImplementation* accept();

		SocketImplementation* accept(const SocketAddress& addr);

		void bindTo(const SocketAddress& bindpoint);

		void connect(const SocketAddress& bindpoint);

		void listen(int maxconn);

		bool read(Packet* pack);

		void readFrom(Packet* pack, SocketAddress* addr);

		void send(Packet* pack);

		void sendTo(Packet* pack, const SocketAddress& addr);

		void close();

		bool hasData();

		bool isDatagramSocket();

		bool isStreamSocket();

		// setters
		void setBlocking(bool status);

		void setLingering(int time);

		void disableLingering();

		inline void setTimeOut(uint64 time) {
			timeout = time;
		}

	protected:
		void createStreamSocket();

		void createDatagramSocket();

		void updateTimeOut();
	};


  } // namespace net
} // namespace sys

using namespace sys::net;

#endif /*SOCKETTIMPLEMENTATION_H_*/
